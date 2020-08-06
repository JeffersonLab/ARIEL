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
#   [wpdir       -              unspecified]
#   [gdmldir     -              gdml]
#   [perllib     -              perl5lib]
#
#   product		version		[<table_format=#>]
#   dependent_product	dependent_product_version [[!]distinguishing qualifier|-|-default-] [optional|only_for_build]
#   ...
#   end_product_list
#
#   qualifier dependent_product1       dependent_product2	notes
#   qual_set1 dependent_product1_qual  dependent_product2_qual	optional notes about this qualifier set
#   qual_set2 dependent_product1_qual  dependent_product2_qual
#   end_qualifier_list
########################################################################
# Notes
#
# * The indir, fcldir, libdir, and bindir lines are optional. Use them
#   only if your product does not conform to the defaults.
# * Format: directory_type directory_path directory_name.
# * The only recognized values of the first field are incdir, fcldir,
#   libdir, and bindir.
# * The only recognized values of the second field are product_dir and
#   fq_dir.
# * The third field is not constrained.
#
# * The start of the product_list section is heralded by a line starting
#   with the word, "product." Other column names are optional and for
#   annotation only. The metadata "<table_format=#>," if specified,
#   describe the schema version for the product list; if omitted, the
#   default schema version is 1. See below for effects.
# * "dependent_product_version" may be the version, "-" or "-c"
#   indicating that the current version of the product should be set up,
#   or "-g<chain>" for the version corresponding to <chain> (note the
#   lack of space).
# * The qualifier specification is a single qualifier (possibly
#   negated), or a "-" indicating that this version should always be
#   used, or "-default-" indicating that this version should be used if
#   no other line's qualifier specification is satisified by the
#   parent's qualifiers.
# * The optional third field should be either "optional" or, "only_for_build."

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

use List::Util qw(min max);     # Numeric min / max functions.

use Exporter 'import';
our (@EXPORT, $compiler_table);

my $default_fc = ( $^O eq "darwin" ) ? "-" : "gfortran";

$compiler_table =
  {
   cc => { CC => "cc", CXX => "c++", FC => $default_fc },
   gcc => { CC => "gcc", CXX => "g++", FC => "gfortran" },
   icc => { CC => "icc", CXX => "icpc", FC => "ifort" },
   clang => { CC => "clang", CXX => "clang++", FC => "gfortran" }
  };

@EXPORT = qw(  annotated_items
               by_version
               cetpkg_info_file
               check_cetbuildtools_version
               check_for_fragment
               check_for_old_noarch_setup_file
               check_for_old_product_deps
               check_for_old_setup_files
               compiler_for_quals
               find_default_qual
               get_bin_directory
               get_fcl_directory
               get_fw_directory
               get_gdml_directory
               get_include_directory
               get_lib_directory
               get_parent_info
               get_perllib
               get_product_list
               get_python_path
               get_qualifier_list
               get_qualifier_matrix
               get_setfw_list
               get_wp_directory
               match_qual
               offset_annotated_items
               parse_version_string
               print_dep_setup
               print_setup_boilerplate
               prods_for_quals
               setup_error
               sort_qual
               to_string
               $compiler_table);

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
    chomp $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      @words = split(/\s+/,$line);
      if ( $words[0] eq "parent" ) {
        $prod=$words[1];
        $ver=$words[2];
        if ( $words[3] ) {
          $extra=$words[3];
        }
      } elsif ( $words[0] eq "defaultqual" ) {
        $dq= sort_qual( $words[1] );
      } elsif ( $words[0] eq "no_fq_dir" ) {
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
    chomp $line;
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
      if ( $words[0] eq "table_fragment_begin" ) {
        $get_fragment="true";
        $frag = "true";
      } elsif ( $words[0] eq "table_fragment_end" ) {
        $get_fragment="";
      } elsif ( $get_fragment ) {
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
    chomp $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      @words = split(/\s+/,$line);
      if ( $words[0] eq "incdir" ) {
        if ( ! $words[2] ) {
          $words[2] = "include";
        }
        if ( $words[1] eq "product_dir" ) {
          $incdir = "\${UPS_PROD_DIR}/".$words[2];
        } elsif ( $words[1] eq "fq_dir" ) {
          $incdir = "\${\${UPS_PROD_NAME_UC}_FQ_DIR}/".$words[2];
        } elsif ( $words[1] eq "-" ) {
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
    chomp $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      @words = split(/\s+/,$line);
      if ( $words[0] eq "bindir" ) {
        if ( ! $words[2] ) {
          $words[2] = "bin";
        }
        if ( $words[1] eq "product_dir" ) {
          $bindir = "\${UPS_PROD_DIR}/".$words[2];
        } elsif ( $words[1] eq "fq_dir" ) {
          $bindir = "\${\${UPS_PROD_NAME_UC}_FQ_DIR}/".$words[2];
        } elsif ( $words[1] eq "-" ) {
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
    chomp $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      @words = split(/\s+/,$line);
      if ( $words[0] eq "libdir" ) {
        if ( ! $words[2] ) {
          $words[2] = "lib";
        }
        if ( $words[1] eq "product_dir" ) {
          $libdir = "\${UPS_PROD_DIR}/".$words[2];
        } elsif ( $words[1] eq "fq_dir" ) {
          $libdir = "\${\${UPS_PROD_NAME_UC}_FQ_DIR}/".$words[2];
        } elsif ( $words[1] eq "-" ) {
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
  my ($pfile, $base_override, $default) = @_;
  my $fcldir = $default || "default";
  my $line;
  my @words;
  open(PIN, "< $pfile") or die "Couldn't open $pfile";
  while ( $line=<PIN> ) {
    chomp $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      @words = split(/\s+/,$line);
      if ( $words[0] eq "fcldir" ) {
        if ( ! $words[2] ) {
          $words[2] = "fcl";
        }
        if ( $words[1] eq "product_dir" ) {
          $fcldir = sprintf("%s/$words[2]",
                            $base_override || '${UPS_PROD_DIR}');
        } elsif ( $words[1] eq "fq_dir" ) {
          $fcldir = sprintf("%s/$words[2]",
                            $base_override || '${${UPS_PROD_NAME_UC}_FQ_DIR}');
        } elsif ( $words[1] eq "-" ) {
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
  my ($pfile, $base_override) = @_;
  my $fwdir = "none";
  my $line;
  my @words;
  open(PIN, "< $pfile") or die "Couldn't open $pfile";
  while ( $line=<PIN> ) {
    chomp $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      @words = split(/\s+/,$line);
      if ( $words[0] eq "fwdir" ) {
        if ( $words[1] eq "-" ) {
          $fwdir = "none";
        } else {
          if ( ! $words[2] ) {
            print "ERROR: the fwdir subdirectory must be specified, there is no default\n";
          }
          if ( $words[1] eq "product_dir" ) {
            $fwdir = sprintf("%s/$words[2]",
                             $base_override || '${UPS_PROD_DIR}');
          } elsif ( $words[1] eq "fq_dir" ) {
            $fwdir = sprintf("%s/$words[2]",
                             $base_override || '${${UPS_PROD_NAME_UC}_FQ_DIR}');
          } else {
            print "ERROR: $words[1] is an invalid directory path\n";
            print "ERROR: directory path must be specified as either \"product_dir\" or \"fq_dir\"\n";
          }
        }
      }
    }
  }
  close(PIN);
  ##print "defining executable directory $fwdir\n";
  return ($fwdir);
}

sub get_wp_directory {
  my @params = @_;
  my $wpdir = "none";
  my $line;
  my @words;
  open(PIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<PIN> ) {
    chomp $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      @words = split(/\s+/,$line);
      if ( $words[0] eq "wpdir" ) {
        if ( $words[1] eq "-" ) {
          $wpdir = "none";
        } else { 
          if ( ! $words[2] ) { 
            print "ERROR: the wpdir subdirectory must be specified, there is no default\n";
          } else {
            if ( $words[1] eq "product_dir" ) {
              $wpdir = "\${UPS_PROD_DIR}/".$words[2];
            } elsif ( $words[1] eq "fq_dir" ) {
              $wpdir = "\${\${UPS_PROD_NAME_UC}_FQ_DIR}/".$words[2];
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
  ##print "defining executable directory $wpdir\n";
  return ($wpdir);
}

sub get_gdml_directory {
  my ($pfile, $base_override) = @_;
  my $gdmldir = "none";
  my $line;
  my @words;
  open(PIN, "< $pfile") or die "Couldn't open $pfile";
  while ( $line=<PIN> ) {
    chomp $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      @words = split(/\s+/,$line);
      if ( $words[0] eq "gdmldir" ) {
        if ( ! $words[2] ) {
          $words[2] = "gdml";
        }
        if ( $words[1] eq "product_dir" ) {
          $gdmldir = sprintf("%s/$words[2]",
                             $base_override || '${UPS_PROD_DIR}');
        } elsif ( $words[1] eq "fq_dir" ) {
          $gdmldir = sprintf("%s/$words[2]",
                             $base_override || '${${UPS_PROD_NAME_UC}_FQ_DIR}');
        } elsif ( $words[1] eq "-" ) {
          $gdmldir = "none";
        } else {
          print "ERROR: $words[1] is an invalid directory path\n";
          print "ERROR: directory path must be specified as either \"product_dir\" or \"fq_dir\"\n";
          print "ERROR: using the default gdml directory path\n";
          $gdmldir = sprintf("%s/$words[2]",
                             $base_override || '${UPS_PROD_DIR}');
        }
      }
    }
  }
  close(PIN);
  ##print "defining executable directory $gdmldir\n";
  return ($gdmldir);
}

sub get_perllib {
  my ($pfile, $base_override) = @_;
  my $prldir = "none";
  my $line;
  my @words;
  open(PIN, "< $pfile") or die "Couldn't open $pfile";
  while ( $line=<PIN> ) {
    chomp $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      @words = split(/\s+/,$line);
      if ( $words[0] eq "perllib" ) {
        if ( ! $words[2] ) {
          $words[2] = "perllib";
        }
        if ( $words[1] eq "product_dir" ) {
          $prldir = sprintf("%s/$words[2]",
                            $base_override || '${UPS_PROD_DIR}');
        } elsif ( $words[1] eq "fq_dir" ) {
          $prldir = sprintf("%s/$words[2]",
                            $base_override || '${${UPS_PROD_NAME_UC}_FQ_DIR}');
        } elsif ( $words[1] eq "-" ) {
          $prldir = "none";
        } else {
          print "ERROR: $words[1] is an invalid directory path\n";
          print "ERROR: directory path must be specified as either \"product_dir\" or \"fq_dir\"\n";
          print "ERROR: using the default perllib directory path\n";
          $prldir = sprintf("%s/$words[2]",
                            $base_override || '${UPS_PROD_DIR}');
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
    chomp $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      my @words = split(/\s+/,$line);
      if ( $words[0] eq "set_fwdir" ) {
        ++$fwiter;
        if ( $words[1] eq "-" ) {
          $setfwdir = "NONE";
          $fwdirs[$fwiter]="NONE";
        } else {
          if ( ! $words[2] ) {
            if ( $words[1] eq "product_dir" ) {
              $setfwdir = "";
              $fwdirs[$fwiter]=$words[1];
            } elsif ( $words[1] eq "fq_dir" ) {
              $setfwdir = "";
              $fwdirs[$fwiter]=$words[1];
            } else {
              $setfwdir = "ERROR";
              $fwdirs[$fwiter]="ERROR";
            }
          } else {
            my $fwsubdir = $words[2];
            if ( $words[1] eq "product_dir" ) {
              $setfwdir = "/$fwsubdir";
              $fwdirs[$fwiter]=$words[1];
            } elsif ( $words[1] eq "fq_dir" ) {
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
    chomp $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      @words = split(/\s+/,$line);
      if ( $words[0] eq "define_pythonpath" ) {
        $pypath = "setme";
      }
    }
  }
  close(PIN);
  ##print "defining executable directory $pypath\n";
  return ($pypath);
}

sub get_product_list {
  my ($pfile) = @_;
  open(PIN, "< $pfile") or die "Couldn't open $pfile";
  my $get_phash="";
  my $pv="";
  my $dqiter=-1;
  my $piter=-1;
  my $i;
  my $line;
  my @words;
  my $phash = {};
  my $pl_format = 1; # Default format.
  while ( $line=<PIN> ) {
    chomp $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      my @words = split(/\s+/,$line);
      if ( $words[0] eq "product" ) {
        $get_phash="true";
        if ($words[$#words] =~ /^<\s*table_format\s*=\s*(\d+)\s*>/o) {
          $pl_format = ${1};
        }
      } elsif ( $words[0] eq "end_product_list" ) {
        $get_phash="";
      } elsif ( $words[0] eq "end_qualifier_list" ) {
        $get_phash="";
      } elsif ( $words[0] eq "parent" ) {
        $get_phash="";
      } elsif ( $words[0] eq "no_fq_dir" ) {
        $get_phash="";
      } elsif ( $words[0] eq "incdir" ) {
        $get_phash="";
      } elsif ( $words[0] eq "fcldir" ) {
        $get_phash="";
      } elsif ( $words[0] eq "gdmldir" ) {
        $get_phash="";
      } elsif ( $words[0] eq "perllib" ) {
        $get_phash="";
      } elsif ( $words[0] eq "fwdir" ) {
        $get_phash="";
      } elsif ( $words[0] eq "libdir" ) {
        $get_phash="";
      } elsif ( $words[0] eq "bindir" ) {
        $get_phash="";
      } elsif ( $words[0] eq "defaultqual" ) {
        $get_phash="";
      } elsif ( $words[0] eq "define_pythonpath" ) {
        $get_phash="";
      } elsif ( $words[0] eq "product" ) {
        $get_phash="";
      } elsif ( $words[0] eq "table_fragment_begin" ) {
        $get_phash="";
      } elsif ( $words[0] eq "table_fragment_end" ) {
        $get_phash="";
      } elsif ( $words[0] eq "table_fragment_begin" ) {
        $get_phash="";
      } elsif ( $words[0] eq "qualifier" ) {
        $get_phash="";
      } elsif ( $get_phash ) {
        # Also covers archaic "only_for_build" lines: do *not* put a
        # special case above.
        ++$piter;
        ##print "get_product_list:  $piter  $words[0] $words[1] $words[2] $words[3]\n";
        my ($prod, $version, $qualspec, $modifier) = @words;

        if ($prod eq "only_for_build") {
          # Archaic form.
          ($prod, $version, $qualspec, $modifier) =
            ($version, $qualspec, '-', $prod);
          print STDERR <<EOF;
WARNING: Deprecated only_for_build entry found in $pfile
WARNING: Please replace:
WARNING: $line
WARNING: with
WARNING: $prod\t$version\t$qualspec\t$modifier
WARNING: This accommodation will be removed in future.
EOF
        }

        if ($pl_format == 1 and $qualspec and $qualspec eq "-nq-") {
          # "Old" meaning of -nq- in a product_list.
          $qualspec = "-";
        }

        $phash->{$prod}->{$qualspec || "-"} =
          {version => (($version eq "-") ? "-c" : $version),
           ($modifier ? ( $modifier => 1 ) : ()) };
      } else {
        ##print "get_product_list: ignoring $line\n";
      }
    }
  }
  close(PIN);
  return $phash;
}

sub prods_for_quals {
  my ($phash, $qualspec) = @_;
  my $results = {};
  foreach my $prod (sort keys %{$phash}) {
    # Find matching version hashes for this product, including default
    # and empty. $phash is the product list hash as produced by
    # get_product_list().
    my $matches =
      { map { match_qual($_, $qualspec) ?
                ( $_ => $phash->{${prod}}->{$_} ) : ();
            } sort keys %{$phash->{$prod}}
      };
    # Remove the default entry from the set of matches (if it exists)
    # and save it.
    my $default = delete $matches->{"-default-"}; # undef if missing.
    if (scalar keys %{$matches} <= 1) { # No more than one match allowed.
      # Use $default if we need to.
      my $hash = (values %{$matches})[0] || $default;
      # Save result if we have one.
      $results->{$prod} = $hash if defined $hash;
    } else {
      die "Ambiguous result matching version for $prod against qualifiers $qualspec";
    }
  }
  return $results;
}

sub get_qualifier_list {
  my @params = @_;
  my $efl = $params[1];
  my $irow=0;
  my $get_quals="false";
  my $line;
  my @words;
  my $qlen = 0;
  my @qlist = ();
  open(QIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<QIN> ) {
    chomp $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      #print $efl "get_qualifier_list: $line\n";
      @words=split(/\s+/,$line);
      if ( $words[0] eq "end_qualifier_list" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "end_product_list" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "parent" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "no_fq_dir" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "incdir" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "fcldir" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "gdmldir" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "perllib" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "fwdir" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "set_fwdir" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "libdir" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "bindir" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "defaultqual" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "only_for_build" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "define_pythonpath" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "product" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "table_fragment_begin" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "table_fragment_end" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "table_fragment_begin" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "qualifier" ) {
        $get_quals="true";
        ##print "qualifiers: $line\n";
        $qlen = $#words;
        for my $i ( 0 .. $#words ) {
          if ( $words[$i] eq "notes" ) {
            $qlen = $i - 1;
          }
        }
        if ( $irow != 0 ) {
          print $efl "echo ERROR: qualifier definition row must come before qualifier list\n";
          print $efl "return 2\n";
          exit 2;
        }
        ##print "there are $qlen product entries out of $#words\n";
        for my $i ( 0 .. $qlen ) {
          $qlist[$irow][$i] = sort_qual( $words[$i] );
        }
        $irow++;
      } elsif ( $get_quals eq "true" ) {
        ##print "$params[0] qualifier $words[0] $#words\n";
        if ( ! $qlen ) {
          print $efl "echo ERROR: qualifier definition row must come before qualifier list\n";
          print $efl "return 3\n";
          exit 3;
        }
        if ( $#words < $qlen ) {
          print $efl "echo ERROR: only $#words qualifiers for $words[0] - need $qlen\n";
          print $efl "return 4\n";
          exit 4;
        }
        for my $i ( 0 .. $qlen ) {
          $qlist[$irow][$i] = sort_qual( $words[$i] );
        }
        $irow++;
      } else {
        ##print "get_qualifier_list: ignoring $line\n";
      }
    }
  }
  close(QIN);
  #print $efl "get_qualifier_list: found $irow qualifier rows\n";
  return ($qlen, @qlist);
}

sub get_qualifier_matrix {
  my @params = @_;
  my $efl = $params[1];
  my $irow=0;
  my $get_quals="false";
  my $line;
  my @words;
  my $qlen = 0;
  my @qlist;
  my $qhash = {}; # By column
  my $qqhash = {}; # By row
  open(QIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<QIN> ) {
    chomp $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      ##print "get_qualifier_matrix: $line\n";
      @words=split(/\s+/,$line);
      if ( $words[0] eq "end_qualifier_list" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "end_product_list" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "parent" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "no_fq_dir" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "incdir" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "fcldir" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "gdmldir" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "perllib" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "fwdir" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "libdir" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "bindir" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "defaultqual" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "only_for_build" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "define_pythonpath" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "product" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "table_fragment_begin" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "table_fragment_end" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "table_fragment_begin" ) {
        $get_quals="false";
      } elsif ( $words[0] eq "qualifier" ) {
        $get_quals="true";
        ##print "get_qualifier_matrix qualifiers: $line\n";
        $qlen = $#words;
        for my $i ( 0 .. $#words ) {
          if ( $words[$i] eq "notes" ) {
            $qlen = $i - 1;
          }
        }
        if ( $irow != 0 ) {
          print $efl "echo ERROR: qualifier definition row must come before qualifier list\n";
          print $efl "return 2\n";
          exit 2;
        }
        ##print "get_qualifier_matrix: there are $qlen product entries out of $#words\n";
        for my $i ( 0 .. $qlen ) {
          $qlist[$i] = $words[$i];
        }
        $irow++;
      } elsif ( $get_quals eq "true" ) {
        #print $efl "get_qualifier_matrix: $params[0] qualifier $words[0] $#words\n";
        if ( ! $qlen ) {
          print $efl "echo ERROR: qualifier definition row must come before qualifier list\n";
          print $efl "return 3\n";
          exit 3;
        }
        if ( $#words < $qlen ) {
          print $efl "echo ERROR: only $#words qualifiers for $words[0] - need $qlen\n";
          print $efl "return 4\n";
          exit 4;
        }
        for my $i ( 1 .. $qlen ) {
          #print $efl "get_qualifier_matrix: fill qhash of $qlist[$i] $words[0] with $words[$i]\n";
          my $parent_qual = sort_qual( $words[0] );
          my $dep_qual = sort_qual( $words[$i] );
          $qhash->{$qlist[$i]}->{$parent_qual} = $dep_qual;
          $qqhash->{$parent_qual}->{$qlist[$i]} = $dep_qual;
        }
        $irow++;
      } else {
        ##print "get_qualifier_matrix: ignoring $line\n";
      }
    }
  }
  close(QIN);
  #print "found $irow qualifier rows\n";
  return ($qlen, $qhash, $qqhash);
}

sub match_qual {
  my ($match_spec, $qualstring) = @_;
  my @quals = split(/:/, $qualstring);
  my ($neg, $qual_spec) = ($match_spec =~ m&^(!)?(.*)$&);
  return ($qual_spec eq '-' or
          $qual_spec eq '-default-' or
          ($neg xor grep { $qual_spec eq $_ } @quals));
}

sub sort_qual {
  my @params = @_;
  my @ql = split(/:/,$params[0]);
  my $retval = 0;
  my @tql = ();
  my @rql = ();
  my $dop="";
  foreach my $ii ( 0 .. $#ql ) {
    if (( $ql[$ii] eq "debug" ) || ( $ql[$ii] eq "opt" )   || ( $ql[$ii] eq "prof" )) {
      $dop=$ql[$ii];
    } else {
      push @tql, $ql[$ii];
    }
  }
  @rql = sort @tql;
  if ( $dop ) {
    push @rql, $dop;
  }
  my $squal = join ( ":", @rql );
  return $squal;
}

sub find_default_qual {
  my @params = @_;
  my $defq = "";
  my $line;
  open(PIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<PIN> ) {
    chomp $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      my @words = split(/\s+/,$line);
      if ( $words[0] eq "defaultqual" ) {
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
    print STDERR "ERROR: cetpkg_info_file found:\n",
      join(", ", @param_vals), ".\n";
    exit(1);
  }
  my $cetpkgfile = "$param_vals[6]/cetpkg_variable_report";
  open(CPG, "> $cetpkgfile") or die "Couldn't open $cetpkgfile";
  print CPG "\n";
  foreach my $index (0 .. $#param_names) {
    my $pval = $param_vals[$index];
    if ( $param_vals[$index] eq "simple" ) {
      $pval = "-";
    }
    printf CPG "CETPKG_%s%s%s\n",
      uc $param_names[$index],  # Var name.
        " " x (max(map { length() + 2 } @param_names) -
               length($param_names[$index])), # Space padding.
                 $pval;                       # Value.
  }
  print CPG "\nTo check cmake cached variables, use cmake -N -L.\n";
  close(CPG);
  return($cetpkgfile);
}

sub check_for_old_product_deps {
  my @params = @_;
  my $retval = 1;
  my $line;
  my @words;
  open(PIN, "< $params[0]") or die "Couldn't open $params[0]";
  while ( $line=<PIN> ) {
    chomp $line;
    if ( index($line,"#") == 0 ) {
    } elsif ( $line !~ /\w+/ ) {
    } else {
      @words = split(/\s+/,$line);
      if ( $words[0] eq "end_product_list" ) {
        $retval = 0;
      } elsif ( $words[0] eq "end_qualifier_list" ) {
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
    chomp $line;
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
    chomp $line;
    if ( $line =~ /simple/ ) {
      $retval = 1;
    }
  }
  close(PIN);
  return $retval;
}

sub setup_error {
  my ($errfl, @msg) = @_;
    open(ERR, "> $errfl") or die "Couldn't open $errfl";
    print ERR <<"EOF";

unsetenv_ CETPKG_NAME
unsetenv_ CETPKG_VERSION
unsetenv_ CETPKG_QUAL
unsetenv_ CETPKG_TYPE
unsetenv_ CETPKG_CC
unsetenv_ CETPKG_CXX
unsetenv_ CETPKG_FC
EOF
  chomp @msg;
  print ERR map { "echo \"ERROR: $_\"\n"; } ("", @msg, "");
  print ERR "return 1\n";
  close(ERR);
  print "$errfl\n";
  exit 0;
}

sub print_dep_setup {
  my ($dep, $dep_info, $dep_quals, $efl, @fail_msg) = @_;
  $dep_info = {} unless defined $dep_info;
  return if ($dep_quals and $dep_quals eq "-");
  my $ql = (not $dep_quals or $dep_quals eq "-b-" or $dep_quals eq "-nq-") ?
    "" :
      sprintf(" -q +\%s", join(":+", split(':', $dep_quals)));
  my $thisver = (not $dep_info->{version} or $dep_info->{version} eq "-") ?
    "" :
      $dep_info->{version};
  print $efl "# > $dep <\n";
  if ($dep_info->{optional}) {
    print $efl <<"EOF";
# setup of $dep is optional
unset have_prod
ups exist $dep $thisver$ql && set_ have_prod="true"
test "\$have_prod" = "true" || echo "INFO: will skip missing optional product $dep $thisver$ql"
test "\$have_prod" = "true" && setup -B $dep $thisver$ql
test "\$?" = 0 || test "\$have_prod" != "true" || set_ setup_fail="true"
unset have_prod
EOF
  } else {
    print $efl <<"EOF";
setup -B $dep $thisver$ql
test "\$?" = 0 || set_ setup_fail="true"
EOF
  }
  foreach my $msg_line ("setup -B $dep $thisver$ql failed", @fail_msg) {
    chomp $msg_line;
    $msg_line =~ s&"&\\"&go;
    print $efl <<EOF;
test "\$setup_fail" = "true" && echo "ERROR: $msg_line"
EOF
  }
  print $efl <<EOF;
test "\$setup_fail" = "true" && return 1 || true
EOF
}

sub compiler_for_quals {
  my ($compilers, $qualspec) = @_;
  my $compiler;
  my @quals = split /:/o, $qualspec;
  if ($compilers->{$qualspec} and $compilers->{$qualspec} ne '-') {
    #print $dfile "product_setup_loop debug info: compiler entry for $qualspec is $compilers->{$qualspec}\n";
    $compiler = $compilers->{$qualspec};
  } elsif (grep /^(?:e13|c\d+)$/o, @quals) {
    $compiler = "clang";
  } elsif (grep /^(?:e|gcc)\d+$/o, @quals) {
    $compiler = "gcc";
  } elsif (grep /^(?:i|icc)\d+$/o, @quals) {
    $compiler = "icc";
  } else {
    $compiler = "cc";           # Native.
  }
  return $compiler;
}

sub offset_annotated_items;

sub to_string {
  my $item = shift;
  $item = (defined $item) ? $item : "<undef>";
  my $indent = shift || 0;
  my $type = ref $item;
  my $result;
  if (not $type) {
    $result = "$item";
  } elsif ($type eq "SCALAR") {
    $result = "$$item";
  } elsif ($type eq "ARRAY") {
    $result = sprintf("\%s ]", offset_annotated_items($indent, '[ ', @$item));
  } elsif ($type eq "HASH") {
    $indent += 2;
    $result =
      sprintf("{ \%s }",
              join(sprintf(",\n\%s", ' ' x $indent),
                   map {
                     my $key = $_;
                     sprintf("$key => \%s",
                             to_string($item->{$key},
                                       $indent + length("$key => ")));
                   } keys %$item));
    $indent -= 2;
  } else {
    print STDERR "ERROR: cannot print item of type $type.\n";
    exit(1);
  }
  return $result;
}

sub offset_annotated_items {
  my ($offset, $preamble, @args) = @_;
  my $indent = length($preamble) + $offset;
  return sprintf('%s%s', $preamble,
                 join(sprintf(",\n\%s", ' ' x $indent),
                      map { to_string($_, $indent); } @args));
}

sub annotated_items {
  return offset_annotated_items(0, @_);
}

# Sort order:
#
# alpha[[-_]NN] (alpha releases);
# beta[[-_]NN] (beta releases);
# rc[[-_]NN] or pre[[-_]NN] (prereleases);
# <empty>;
# p[-_]NN or patch[[-_]NN] (patch releases);
# Anything else.
sub parse_version_extra {
  my $vInfo = shift;
  # Swallow optional _ or - separator to 4th field.
  $vInfo->{micro}=~ m&^(\d+)[-_]?((.*?)[-_]?(\d*))$&o;
  $vInfo->{micro} = ${1};
  my ($extra, $etext, $enum) = (${2} || "", ${3} || "", ${4} || -1);
  if (not $etext) {
    $vInfo->{extra_type} = 0;
  } elsif ($etext eq "patch" or ($enum >= 0 and $etext eq "p")) {
    $vInfo->{extra_type} = 1;
  } elsif ($etext eq "rc" or
           $etext eq "pre") {
    $vInfo->{extra_type} = -1;
    $etext = "pre";
  } elsif ($etext eq "beta") {
    $vInfo->{extra_type} = -2;
  } elsif ($etext eq "alpha") {
    $vInfo->{extra_type} = -3;
  } else {
    $vInfo->{extra_type} = 2;
  }
  $vInfo->{extra_num} = $enum;
  $vInfo->{extra_text} = $etext;
}

sub parse_version_string {
  my $dv = shift || "";
  $dv =~ s&^v&&o;
  my $result = {};
  if ($dv) {
    @{$result}{qw(major minor micro)} = split /[_.]/, $dv, 3;
    parse_version_extra($result);
  } else {
    @{$result}{qw(major minor micro extra_type extra_text extra_num)} =
      (-1, -1, -1, 0, "", -1);
  }
  return $result;
}

sub by_version {
  # Requires dot versions.
  my $vInfoA = parse_version_string($a || shift);
  my $vInfoB = parse_version_string($b || shift);
  return
    $vInfoA->{major} <=> $vInfoB->{major} ||
      $vInfoA->{minor} <=> $vInfoB->{minor} ||
        $vInfoA->{micro} <=> $vInfoB->{micro} ||
          $vInfoA->{extra_type} <=> $vInfoB->{extra_type} ||
            $vInfoA->{extra_text} cmp $vInfoB->{extra_text} ||
              $vInfoA->{extra_num} <=> $vInfoB->{extra_num};
}

sub print_setup_boilerplate {
  my ($fh, $product, $version, $noarch, $qual, $type, $compiler) = @_;
  print $fh <<EOF;
set_ setup_fail="false"
setenv CETPKG_NAME $product
setenv CETPKG_VERSION $version
EOF
  if ($noarch) {
    print $fh <<EOF;
unsetenv_ CETPKG_QUAL
unsetenv_ CETPKG_TYPE
unsetenv_ CETPKG_CC
unsetenv_ CETPKG_CXX
unsetenv_ CETPKG_FC
EOF
  } else {
    print $fh <<EOF;
setenv CETPKG_QUAL $qual
setenv CETPKG_TYPE $type
setenv CETPKG_CC $compiler_table->{${compiler}}->{CC}
setenv CETPKG_CXX $compiler_table->{${compiler}}->{CXX}
setenv CETPKG_FC $compiler_table->{${compiler}}->{FC}
EOF
  }
  print_dep_setup("cetpkgsupport", {}, "-nq-", $fh);
}
