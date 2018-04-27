# parse product_deps

# product_deps format:

#   parent       this_product   this_version
#   defaultqual  qualifier
#
#   [incdir      product_dir    include]
#   [fcldir      product_dir    fcl]
#   [libdir      fq_dir	        lib]
#   [bindir      fq_dir         bin]
#   [fwdir       -              unspecified]
#   [gdmldir     -              gdml]
#   [perllib     -              perl5lib]
#
#   product		version
#   dependent_product	dependent_product_version [distinguishing qualifier|-] [optional|only_for_build]
#
#   qualifier dependent_product1       dependent_product2	notes
#   qual_set1 dependent_product1_qual  dependent_product2_qual	optional notes about this qualifier set
#   qual_set2 dependent_product1_qual  dependent_product2_qual

# The indir, fcldir, libdir, and bindir lines are optional
# Use them only if your product does not conform to the defaults
# Format: directory_type directory_path directory_name
# The only recognized values of the first field are incdir, fcldir, libdir, and bindir
# The only recognized values of the second field are product_dir and fq_dir
# The third field is not constrained
#
# if dependent_product_version is a dash, the "current" version will be specified
# If a dependent product is optional, then add "optional" to the third field. 

#
# Use as many rows as you need for the qualifiers
# Use a separate column for each dependent product that must be explicitly setup
# Do not list products which will be setup by a dependent_product
#
# special qualifier options
# -	not installed for this parent qualifier
# -nq-	this dependent product has no qualifier
# -b-	this dependent product is only used for the build - it will not be in the table

use strict;
use warnings;

package parse_deps;

use List::Util qw(min max); # Numeric min / max funcions.

use Exporter 'import';
our (@EXPORT, @setup_list);
@EXPORT = qw(  get_parent_info 
	       check_for_fragment 
               compare_versions 
	       get_include_directory 
	       get_bin_directory 
	       get_lib_directory 
	       get_fcl_directory 
	       get_fw_directory 
	       get_setfw_list
	       get_gdml_directory 
	       get_perllib 
	       get_python_path 
	       get_product_list 
	       get_qualifier_list 
	       get_qualifier_matrix 
	       compare_qual 
	       match_qual 
	       sort_qual 
	       check_flags 
	       find_default_qual 
	       cetpkg_info_file 
	       print_setup_noqual 
	       print_setup_qual 
               set_print_command
	       check_cetbuildtools_version 
	       check_for_old_product_deps
	       check_for_old_setup_files
	       check_for_old_noarch_setup_file
               @setup_list);

sub get_parent_info {
  my @params = @_;
  open(PIN, "< $params[0]") or die "Couldn't open $params[0]";
  my $extra="none";
  my $line;
  my @words;
  my $prod;
  my $ver; 
  my $fq = "true";
  my $dq = "-nq-";
  while ( $line=<PIN> ) {
    chop $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      @words = split(/\s+/,$line);
      if( $words[0] eq "parent" ) {
	 $prod=$words[1];
	 $ver=$words[2];
	 if( $words[3] ) { $extra=$words[3]; }
      } elsif( $words[0] eq "defaultqual" ) {
	 $dq= sort_qual( $words[1] );
      } elsif( $words[0] eq "no_fq_dir" ) {
          $fq = "";
      } else {
        ##print "get_parent_info: ignoring $line\n";
      }
    }
  }
  close(PIN);
  return ($prod, $ver, $extra, $dq, $fq);
}

sub check_for_fragment {
  my @params = @_;
  my $frag = "";
  my $get_fragment="";
  my $nfrag=0;
  my $line;
  my @words;
  my @fraglines;
  open(PIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<PIN> ) {
    chop $line;
    if ( index($line,"#") == 0 ) {
      # comments might be part of a table fragment
      if ( $get_fragment ) {
	#print "found fragment $line\n";
	$fraglines[$nfrag] = $line;
	++$nfrag;
      }
    } elsif ( $line !~ /\w+/ ) {
      # empty lines might be part of a table fragment
      if ( $get_fragment ) {
	#print "found fragment $line\n";
	$fraglines[$nfrag] = $line;
	++$nfrag;
      }
    } else {
      @words = split(/\s+/,$line);
      if( $words[0] eq "table_fragment_begin" ) {
         $get_fragment="true";
         $frag = "true";
      } elsif( $words[0] eq "table_fragment_end" ) {
         $get_fragment="";
      } elsif( $get_fragment ) {
	#print "found fragment $line\n";
	$fraglines[$nfrag] = $line;
	++$nfrag;
      } else {
      }
    }
  }
  close(PIN);
  #print "found $nfrag table fragment lines\n";
  return ($frag,@fraglines);
}

sub get_include_directory {
  my @params = @_;
  my $incdir = "default";
  my $line;
  my @words;
  open(PIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<PIN> ) {
    chop $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      @words = split(/\s+/,$line);
      if( $words[0] eq "incdir" ) {
         if( ! $words[2] ) { $words[2] = "include"; }
         if( $words[1] eq "product_dir" ) {
	    $incdir = "\${UPS_PROD_DIR}/".$words[2];
         } elsif( $words[1] eq "fq_dir" ) {
	    $incdir = "\${\${UPS_PROD_NAME_UC}_FQ_DIR}/".$words[2];
         } elsif( $words[1] eq "-" ) {
	    $incdir = "none";
	 } else {
	    print "ERROR: $words[1] is an invalid directory path\n";
	    print "ERROR: directory path must be specified as either \"product_dir\" or \"fq_dir\"\n";
	    print "ERROR: using the default include directory path\n";
	 }
      }
    }
  }
  close(PIN);
  ##print "defining include directory $incdir\n";
  return ($incdir);
}

sub get_bin_directory {
  my @params = @_;
  my $bindir = "default";
  my $line;
  my @words;
  open(PIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<PIN> ) {
    chop $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      @words = split(/\s+/,$line);
      if( $words[0] eq "bindir" ) {
         if( ! $words[2] ) { $words[2] = "bin"; }
         if( $words[1] eq "product_dir" ) {
	    $bindir = "\${UPS_PROD_DIR}/".$words[2];
         } elsif( $words[1] eq "fq_dir" ) {
	    $bindir = "\${\${UPS_PROD_NAME_UC}_FQ_DIR}/".$words[2];
         } elsif( $words[1] eq "-" ) {
	    $bindir = "none";
	 } else {
	    print "ERROR: $words[1] is an invalid directory path\n";
	    print "ERROR: directory path must be specified as either \"product_dir\" or \"fq_dir\"\n";
	    print "ERROR: using the default bin directory path\n";
	 }
      }
    }
  }
  close(PIN);
  ##print "defining executable directory $bindir\n";
  return ($bindir);
}

sub get_lib_directory {
  my @params = @_;
  my $libdir = "default";
  my $line;
  my @words;
  open(PIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<PIN> ) {
    chop $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      @words = split(/\s+/,$line);
      if( $words[0] eq "libdir" ) {
         if( ! $words[2] ) { $words[2] = "lib"; }
         if( $words[1] eq "product_dir" ) {
	    $libdir = "\${UPS_PROD_DIR}/".$words[2];
         } elsif( $words[1] eq "fq_dir" ) {
	    $libdir = "\${\${UPS_PROD_NAME_UC}_FQ_DIR}/".$words[2];
         } elsif( $words[1] eq "-" ) {
	    $libdir = "none";
	 } else {
	    print "ERROR: $words[1] is an invalid directory path\n";
	    print "ERROR: directory path must be specified as either \"product_dir\" or \"fq_dir\"\n";
	    print "ERROR: using the default include directory path\n";
	 }
      }
    }
  }
  close(PIN);
  ##print "defining library directory $libdir\n";
  return ($libdir);
}

sub get_fcl_directory {
  my @params = @_;
  my $fcldir = "default";
  my $line;
  my @words;
  open(PIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<PIN> ) {
    chop $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      @words = split(/\s+/,$line);
      if( $words[0] eq "fcldir" ) {
         if( ! $words[2] ) { $words[2] = "fcl"; }
         if( $words[1] eq "product_dir" ) {
	    $fcldir = "\${UPS_PROD_DIR}/".$words[2];
         } elsif( $words[1] eq "fq_dir" ) {
	    $fcldir = "\${\${UPS_PROD_NAME_UC}_FQ_DIR}/".$words[2];
         } elsif( $words[1] eq "-" ) {
	    $fcldir = "none";
	 } else {
	    print "ERROR: $words[1] is an invalid directory path\n";
	    print "ERROR: directory path must be specified as either \"product_dir\" or \"fq_dir\"\n";
	    print "ERROR: using the default fcl directory path\n";
	 }
      }
    }
  }
  close(PIN);
  ##print "defining executable directory $fcldir\n";
  return ($fcldir);
}

sub get_fw_directory {
  my @params = @_;
  my $fwdir = "none";
  my $line;
  my @words;
  open(PIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<PIN> ) {
    chop $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      @words = split(/\s+/,$line);
      if( $words[0] eq "fwdir" ) {
         if( $words[1] eq "-" ) {
	    $fwdir = "none";
	 } else { 
            if( ! $words[2] ) { 
		  print "ERROR: the fwdir subdirectory must be specified, there is no default\n";
	    } else {
               if( $words[1] eq "product_dir" ) {
		  $fwdir = "\${UPS_PROD_DIR}/".$words[2];
               } elsif( $words[1] eq "fq_dir" ) {
		  $fwdir = "\${\${UPS_PROD_NAME_UC}_FQ_DIR}/".$words[2];
	       } else {
		  print "ERROR: $words[1] is an invalid directory path\n";
		  print "ERROR: directory path must be specified as either \"product_dir\" or \"fq_dir\"\n";
	       }
	    }
	 }
      }
    }
  }
  close(PIN);
  ##print "defining executable directory $fwdir\n";
  return ($fwdir);
}

sub get_gdml_directory {
  my @params = @_;
  my $gdmldir = "none";
  my $line;
  my @words;
  open(PIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<PIN> ) {
    chop $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      @words = split(/\s+/,$line);
      if( $words[0] eq "gdmldir" ) {
         if( ! $words[2] ) { $words[2] = "gdml"; }
         if( $words[1] eq "product_dir" ) {
	    $gdmldir = "\${UPS_PROD_DIR}/".$words[2];
         } elsif( $words[1] eq "fq_dir" ) {
	    $gdmldir = "\${\${UPS_PROD_NAME_UC}_FQ_DIR}/".$words[2];
         } elsif( $words[1] eq "-" ) {
	    $gdmldir = "none";
	 } else {
	    print "ERROR: $words[1] is an invalid directory path\n";
	    print "ERROR: directory path must be specified as either \"product_dir\" or \"fq_dir\"\n";
	    print "ERROR: using the default gdml directory path\n";
	    $gdmldir = "\${UPS_PROD_DIR}/".$words[2];
	 }
      }
    }
  }
  close(PIN);
  ##print "defining executable directory $gdmldir\n";
  return ($gdmldir);
}

sub get_perllib {
  my @params = @_;
  my $prldir = "none";
  my $line;
  my @words;
  open(PIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<PIN> ) {
    chop $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      @words = split(/\s+/,$line);
      if( $words[0] eq "perllib" ) {
         if( ! $words[2] ) { $words[2] = "perllib"; }
         if( $words[1] eq "product_dir" ) {
	    $prldir = "\${UPS_PROD_DIR}/".$words[2];
         } elsif( $words[1] eq "fq_dir" ) {
	    $prldir = "\${\${UPS_PROD_NAME_UC}_FQ_DIR}/".$words[2];
         } elsif( $words[1] eq "-" ) {
	    $prldir = "none";
	 } else {
	    print "ERROR: $words[1] is an invalid directory path\n";
	    print "ERROR: directory path must be specified as either \"product_dir\" or \"fq_dir\"\n";
	    print "ERROR: using the default perllib directory path\n";
	    $prldir = "\${UPS_PROD_DIR}/".$words[2];
	 }
      }
    }
  }
  close(PIN);
  ##print "defining executable directory $prldir\n";
  return ($prldir);
}

# This list is meant to point to the source code directory
sub get_setfw_list {
  my @params = @_;
  my $setfwdir = "NONE";
  my @fwlist;
  my @fwdirs;
  my $fwiter=-1;
  my $line;
  open(PIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<PIN> ) {
    chop $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      my @words = split(/\s+/,$line);
      if( $words[0] eq "set_fwdir" ) {
         ++$fwiter;
         if( $words[1] eq "-" ) {
	     $setfwdir = "NONE";
             $fwdirs[$fwiter]="NONE";
	 } else { 
            if( ! $words[2] ) { 
               if( $words[1] eq "product_dir" ) {
		  $setfwdir = "";
		  $fwdirs[$fwiter]=$words[1];
               } elsif( $words[1] eq "fq_dir" ) {
		  $setfwdir = "";
		  $fwdirs[$fwiter]=$words[1];
	       } else {
		  $setfwdir = "ERROR";
		  $fwdirs[$fwiter]="ERROR";
	       }
	    } else {
	       my $fwsubdir = $words[2];
               if( $words[1] eq "product_dir" ) {
		  $setfwdir = "/$fwsubdir";
		  $fwdirs[$fwiter]=$words[1];
               } elsif( $words[1] eq "fq_dir" ) {
		  $fwdirs[$fwiter]=$words[1];
		  $setfwdir = "/$fwsubdir";
	       } else {
		  $setfwdir = "ERROR";
		  $fwdirs[$fwiter]="ERROR";
	       }
	    }
	 }
	 $fwlist[$fwiter]=$setfwdir;
      }
    }
  }
  close(PIN);
  return ($fwiter, \@fwlist, \@fwdirs);
}

sub get_python_path {
  my @params = @_;
  my $pypath = "none";
  my $line;
  my @words;
  open(PIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<PIN> ) {
    chop $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      @words = split(/\s+/,$line);
      if( $words[0] eq "define_pythonpath" ) {
            $pypath = "setme";
	 }
    }
  }
  close(PIN);
  ##print "defining executable directory $pypath\n";
  return ($pypath);
}

sub get_product_list {
  my @params = @_;
  open(PIN, "< $params[0]") or die "Couldn't open $params[0]";
  my $get_phash="";
  my $pv="";
  my $dqiter=-1;
  my $piter=-1;
  my $i;
  my $line;
  my @plist;
  my @words;
  my @dplist;
  while ( $line=<PIN> ) {
    chop $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      my @words = split(/\s+/,$line);
      if( $words[0] eq "product" ) {
	 $get_phash="true";
      } elsif( $words[0] eq "end_product_list" ) {
	 $get_phash="";
      } elsif( $words[0] eq "end_qualifier_list" ) {
         $get_phash="";
      } elsif( $words[0] eq "parent" ) {
         $get_phash="";
      } elsif( $words[0] eq "no_fq_dir" ) {
         $get_phash="";
      } elsif( $words[0] eq "incdir" ) {
         $get_phash="";
      } elsif( $words[0] eq "fcldir" ) {
         $get_phash="";
      } elsif( $words[0] eq "gdmldir" ) {
         $get_phash="";
      } elsif( $words[0] eq "perllib" ) {
         $get_phash="";
      } elsif( $words[0] eq "fwdir" ) {
         $get_phash="";
      } elsif( $words[0] eq "libdir" ) {
         $get_phash="";
      } elsif( $words[0] eq "bindir" ) {
         $get_phash="";
      } elsif( $words[0] eq "defaultqual" ) {
         $get_phash="";
      } elsif( $words[0] eq "only_for_build" ) {
         $get_phash="";
      } elsif( $words[0] eq "define_pythonpath" ) {
         $get_phash="";
      } elsif( $words[0] eq "product" ) {
         $get_phash="";
      } elsif( $words[0] eq "table_fragment_begin" ) {
         $get_phash="";
      } elsif( $words[0] eq "table_fragment_end" ) {
         $get_phash="";
      } elsif( $words[0] eq "table_fragment_begin" ) {
         $get_phash="";
      } elsif( $words[0] eq "qualifier" ) {
         $get_phash="";
      } elsif( $get_phash ) {
        if(( $words[2] ) && ($words[2]eq "-" )) { $words[2] = ""; }
	++$piter;
        ##print "get_product_list:  $piter  $words[0] $words[1] $words[2] $words[3]\n";
	for $i ( 0 .. $#words ) {
	  $plist[$piter][$i] = $words[$i];
	}
	if( $words[2] ) {
	  my $have_match="false";
	  for $i ( 0 .. $dqiter ) {
	    if( $dplist[$i] eq $words[2] )  { $have_match="true"; }
	  }
	  if ( $have_match eq "false" ) {
	    ++$dqiter;
	    $dplist[$dqiter]=$words[2];
	  }
	}
      } else {
        ##print "get_product_list: ignoring $line\n";
      }
    }
  }
  close(PIN);
  return ($piter, \@plist, $dqiter, \@dplist);
}

sub get_qualifier_list {
  my @params = @_;
  my $efl = $params[1];
  my $irow=0;
  my $get_quals="false";
  my $i;
  my $line;
  my @words;
  my $qlen = 0;
  my @qlist = ();
  open(QIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<QIN> ) {
    chop $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      ##print "get_qualifier_list: $line\n";
      @words=split(/\s+/,$line);
      if( $words[0] eq "end_qualifier_list" ) {
         $get_quals="false";
      } elsif( $words[0] eq "end_product_list" ) {
         $get_quals="false";
      } elsif( $words[0] eq "parent" ) {
         $get_quals="false";
      } elsif( $words[0] eq "no_fq_dir" ) {
         $get_quals="false";
      } elsif( $words[0] eq "incdir" ) {
         $get_quals="false";
      } elsif( $words[0] eq "fcldir" ) {
         $get_quals="false";
      } elsif( $words[0] eq "gdmldir" ) {
         $get_quals="false";
      } elsif( $words[0] eq "perllib" ) {
         $get_quals="false";
      } elsif( $words[0] eq "fwdir" ) {
         $get_quals="false";
      } elsif( $words[0] eq "libdir" ) {
         $get_quals="false";
      } elsif( $words[0] eq "bindir" ) {
         $get_quals="false";
      } elsif( $words[0] eq "defaultqual" ) {
         $get_quals="false";
      } elsif( $words[0] eq "only_for_build" ) {
         $get_quals="false";
      } elsif( $words[0] eq "define_pythonpath" ) {
         $get_quals="false";
      } elsif( $words[0] eq "product" ) {
         $get_quals="false";
      } elsif( $words[0] eq "table_fragment_begin" ) {
         $get_quals="false";
      } elsif( $words[0] eq "table_fragment_end" ) {
         $get_quals="false";
      } elsif( $words[0] eq "table_fragment_begin" ) {
         $get_quals="false";
      } elsif( $words[0] eq "qualifier" ) {
         $get_quals="true";
         ##print "qualifiers: $line\n";
	 $qlen = $#words;
	 for $i ( 0 .. $#words ) {
	      if( $words[$i] eq "notes" ) {
		 $qlen = $i - 1;
	      }
	 }
	 if( $irow != 0 ) {
            print $efl "echo ERROR: qualifier definition row must come before qualifier list\n";
            print $efl "return 2\n";
	    exit 2;
	 }
	 ##print "there are $qlen product entries out of $#words\n";
	 for $i ( 0 .. $qlen ) {
	   $qlist[$irow][$i] = sort_qual( $words[$i] );
	 }
	 $irow++;
      } elsif( $get_quals eq "true" ) {
	 ##print "$params[0] qualifier $words[0] $#words\n";
	 if( ! $qlen ) {
            print $efl "echo ERROR: qualifier definition row must come before qualifier list\n";
            print $efl "return 3\n";
	    exit 3;
	 }
	 if ( $#words < $qlen ) {
            print $efl "echo ERROR: only $#words qualifiers for $words[0] - need $qlen\n";
            print $efl "return 4\n";
	    exit 4;
	 }
	 for $i ( 0 .. $qlen ) {
	   $qlist[$irow][$i] = sort_qual( $words[$i] );
	 }
	 $irow++;
      } else {
        ##print "get_qualifier_list: ignoring $line\n";
      }
    }
  }
  close(QIN);
  ##print "found $irow qualifier rows\n";
  return ($qlen, @qlist);
}

sub get_qualifier_matrix {
  my @params = @_;
  my $efl = $params[1];
  my $irow=0;
  my $get_quals="false";
  my $i;
  my $line;
  my @words;
  my $qlen = 0;
  my @qlist;
  my %qhash = ();
  open(QIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<QIN> ) {
    chop $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      ##print "get_qualifier_matrix: $line\n";
      @words=split(/\s+/,$line);
      if( $words[0] eq "end_qualifier_list" ) {
         $get_quals="false";
      } elsif( $words[0] eq "end_product_list" ) {
         $get_quals="false";
      } elsif( $words[0] eq "parent" ) {
         $get_quals="false";
      } elsif( $words[0] eq "no_fq_dir" ) {
         $get_quals="false";
      } elsif( $words[0] eq "incdir" ) {
         $get_quals="false";
      } elsif( $words[0] eq "fcldir" ) {
         $get_quals="false";
      } elsif( $words[0] eq "gdmldir" ) {
         $get_quals="false";
      } elsif( $words[0] eq "perllib" ) {
         $get_quals="false";
      } elsif( $words[0] eq "fwdir" ) {
         $get_quals="false";
      } elsif( $words[0] eq "libdir" ) {
         $get_quals="false";
      } elsif( $words[0] eq "bindir" ) {
         $get_quals="false";
      } elsif( $words[0] eq "defaultqual" ) {
         $get_quals="false";
      } elsif( $words[0] eq "only_for_build" ) {
         $get_quals="false";
      } elsif( $words[0] eq "define_pythonpath" ) {
         $get_quals="false";
      } elsif( $words[0] eq "product" ) {
         $get_quals="false";
      } elsif( $words[0] eq "table_fragment_begin" ) {
         $get_quals="false";
      } elsif( $words[0] eq "table_fragment_end" ) {
         $get_quals="false";
      } elsif( $words[0] eq "table_fragment_begin" ) {
         $get_quals="false";
      } elsif( $words[0] eq "qualifier" ) {
         $get_quals="true";
         ##print "get_qualifier_matrix qualifiers: $line\n";
	 $qlen = $#words;
	 for $i ( 0 .. $#words ) {
	      if( $words[$i] eq "notes" ) {
		 $qlen = $i - 1;
	      }
	 }
	 if( $irow != 0 ) {
            print $efl "echo ERROR: qualifier definition row must come before qualifier list\n";
            print $efl "return 2\n";
	    exit 2;
	 }
	 ##print "get_qualifier_matrix: there are $qlen product entries out of $#words\n";
	 for $i ( 0 .. $qlen ) {
	   $qlist[$i] = $words[$i];
	 }
	 $irow++;
      } elsif( $get_quals eq "true" ) {
	 ##print "get_qualifier_matrix: $params[0] qualifier $words[0] $#words\n";
	 if( ! $qlen ) {
            print $efl "echo ERROR: qualifier definition row must come before qualifier list\n";
            print $efl "return 3\n";
	    exit 3;
	 }
	 if ( $#words < $qlen ) {
            print $efl "echo ERROR: only $#words qualifiers for $words[0] - need $qlen\n";
            print $efl "return 4\n";
	    exit 4;
	 }
	 for $i ( 1 .. $qlen ) {
	   $qhash{$qlist[$i]}->{sort_qual( $words[0] )} = sort_qual( $words[$i] );
	 }
	 $irow++;
      } else {
        ##print "get_qualifier_matrix: ignoring $line\n";
      }
    }
  }
  close(QIN);
  ##print "found $irow qualifier rows\n";
  return ($qlen, %qhash);
}

# compare_qual is obsolete
sub compare_qual {
  my @params = @_;
  my @ql1 = split(/:/,$params[0]);
  my @ql2 = split(/:/,$params[1]);
  my $retval = 0;
  if( $#ql1 != $#ql2 ) { return $retval; }
  my $size = $#ql2 + 1;
  my $qmatch = 0;
  my $ii;
  my $jj;
  foreach $ii ( 0 .. $#ql1 ) {
    foreach $jj ( 0 .. $#ql2 ) {
      if( $ql1[$ii] eq $ql2[$jj] )  { $qmatch++; }
    }
  }
  if( $qmatch == $size ) { $retval = 1; }
  return $retval;
}

sub match_qual {
  my @params = @_;
  my $q1 = $params[0];
  my @ql2 = split(/:/,$params[1]);
  my $retval = 0;
  my $ii;
  foreach $ii ( 0 .. $#ql2 ) {
      if( $q1 eq $ql2[$ii] )  { $retval = 1; }
  }
  return $retval;
}

sub sort_qual {
  my @params = @_;
  my @ql = split(/:/,$params[0]);
  my $retval = 0;
  my @tql = ();
  my @rql = ();
  my $dop="";
  foreach my $ii ( 0 .. $#ql ) {
      if(( $ql[$ii] eq "debug" ) || ( $ql[$ii] eq "opt" )   || ( $ql[$ii] eq "prof" )) {
         $dop=$ql[$ii];
      } else {
         push @tql, $ql[$ii];
      }
  }
  @rql = sort @tql;
  if( $dop ) { push @rql, $dop; }
  my $squal = join ( ":", @rql );
  return $squal;
}

sub check_flags {
  my @params = @_;
  my $type = uc $params[1];
  my $cxxflg = "";
  my $cflg = "";
  my $line;
  open(PIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<PIN> ) {
    chop $line;
    my @words = split(/\s+/,$line);
    if ( $words[0] eq "CET_BASE_CXX_FLAG_${type}:" ) {
       $cxxflg = $words[1];
    } elsif ( $words[0] eq "CET_BASE_C_FLAG_${type}:" ) {
       $cflg = $words[1];
    }
  }
  close(PIN);
  return ($cxxflg,$cflg);
}

sub find_default_qual {
  my @params = @_;
  my $defq = "";
  my $line;
  open(PIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<PIN> ) {
    chop $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      my @words = split(/\s+/,$line);
      if( $words[0] eq "defaultqual" ) {
         $defq = sort_qual( $words[1] );
      }
    }
  }
  close(PIN);
  ##print "defining library directory $libdir\n";
  return ($defq);
}

sub cetpkg_info_file {
  ## write a file to be processed by CetCMakeEnv
  ## add CETPKG_SOURCE and CETPKG_BUILD for ease of reference by the user
  # if there is a cmake cache file, we could check for the install prefix
  # cmake -N -L | grep CMAKE_INSTALL_PREFIX | cut -f2 -d=
  my @param_names =
    qw (name version default_version qual type source build cc cxx fc only_for_build);
  my @param_vals = @_;
  if (scalar @param_vals != scalar @param_names) {
    print STDERR "ERROR: cetpkg_info_file expects the following paramaters in order:\n",
      join(", ", @param_names), ".\n";
    exit(1);
  }
  my $cetpkgfile = "$param_vals[6]/cetpkg_variable_report";
  open(CPG, "> $cetpkgfile") or die "Couldn't open $cetpkgfile";
  print CPG "\n";
  foreach my $index (0 .. $#param_names) {
    printf CPG "CETPKG_%s%s%s\n",
      uc $param_names[$index], # Var name.
        " " x (max(map { length() + 2 } @param_names) -
               length($param_names[$index])), # Space padding.
          $param_vals[$index]; # Value.
  }
  print CPG "\nTo check cmake cached variables, use cmake -N -L.\n";
  close(CPG);
  return($cetpkgfile);
}

sub print_setup_noqual {
  my @params = @_;
  my $efl = $params[3];
  my $thisqual = $params[1];
  if( $params[1] eq "-" ) {  $thisqual = ""; }
  if(( $params[2] ) && ( $params[2] eq "optional" )) { 
  print $efl "# setup of $params[0] is optional\n"; 
  print $efl "unset have_prod\n"; 
  print $efl "ups exist $params[0] $thisqual\n"; 
  print $efl "test \"\$?\" = 0 && set_ have_prod=\"true\"\n"; 
  print $efl "test \"\$have_prod\" = \"true\" || echo \"will not setup $params[0] $thisqual\"\n"; 
  print $efl "test \"\$have_prod\" = \"true\" && setup -B $params[0] $thisqual \n";
  print $efl "unset have_prod\n"; 
  } else {
  print $efl "setup -B $params[0] $thisqual \n";
  print $efl "test \"\$?\" = 0 || set_ setup_fail=\"true\"\n"; 
  }
  return 0;
}

sub print_setup_qual {
  my @params = @_;
  my $efl = $params[4];
  my $thisqual = $params[1];
  if( $params[1] eq "-" ) {  $thisqual = ""; }
  if(( $params[3] ) && ( $params[3] eq "optional" )) { 
  print $efl "# setup of $params[0] is optional\n"; 
  print $efl "unset have_prod\n"; 
  print $efl "ups exist $params[0] $thisqual -q $params[2]\n"; 
  print $efl "test \"\$?\" = 0 && set_ have_prod=\"true\"\n"; 
  print $efl "test \"\$have_prod\" = \"true\" || echo \"will not setup $params[0] $thisqual -q $params[2]\"\n"; 
  print $efl "test \"\$have_prod\" = \"true\" && setup -B $params[0] $thisqual -q $params[2] \n";
  print $efl "unset have_prod\n"; 
  } else {
  print $efl "setup -B $params[0] $thisqual -q $params[2]\n";
  print $efl "test \"\$?\" = 0 || set_ setup_fail=\"true\"\n"; 
	#print TSET "setup -B $qlist[0][$j] $phash{$qlist[0][$j]} -q $ql \n";
  }
  return 0;
}

# set_print_command( $plist[$i][0], $plist[$i][1], $plist[$i][3], product_qual, $set_dev_products::SETUP_CMDS);
sub set_print_command {
  my @params = @_;
  my $efl = $params[4];
  my $pql = $params[3];
	if ( $pql eq "-" ) {
	} elsif ( $pql eq "-nq-" ) {
          print_setup_noqual( $params[0], $params[1], $params[2], $efl );
	} elsif ( $pql eq "-b-" ) {
          print_setup_noqual( $params[0], $params[1], $params[2], $efl );
	} else {
          my @qwords = split(/:/,$pql);
          my $ql="+".$qwords[0];
	  my $j;
          foreach $j ( 1 .. $#qwords ) {
            $ql = $ql.":+".$qwords[$j];
          }
          print_setup_qual( $params[0], $params[1], $ql, $params[2], $efl );
	}
}

sub check_for_old_product_deps {
  my @params = @_;
  my $retval = 1;
  my $line;
  my @words;
  open(PIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<PIN> ) {
    chop $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      @words = split(/\s+/,$line);
      if( $words[0] eq "end_product_list" ) {
            $retval = 0;
      } elsif( $words[0] eq "end_qualifier_list" ) {
            $retval = 0;
      }
    }
  }
  close(PIN);
  return $retval;
}

sub check_for_old_setup_files {
  my @params = @_;
  my $retval = 0;
  my $line;
  open(PIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<PIN> ) {
    chop $line;
    if ( $line =~ /UPS_OVERRIDE/ ) {
            $retval = 1;
    }
  }
  close(PIN);
  return $retval;
}

sub check_for_old_noarch_setup_file {
  my @params = @_;
  my $retval = 0;
  my $line;
  open(PIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<PIN> ) {
    chop $line;
    if ( $line =~ /simple/ ) {
            $retval = 1;
    }
  }
  close(PIN);
  return $retval;
}

1;
