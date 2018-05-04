use strict;

use vars qw(%dir_list);
BEGIN { %dir_list = (
        "art_Persistency_Common" => "art_Persistency_Common canvas",
        "art_Persistency_Provenance" => "art_Persistency_Provenance canvas",
        "art_Persistency_Common_dict" => "canvas_Persistency_Common_dict",
        "art_Persistency_StdDictionaries" => "canvas_Persistency_StdDictionaries",
        "art_Persistency_StdDictionaries_dict" => "canvas_Persistency_StdDictionaries_dict",
        "art_Persistency_WrappedStdDictionaries" => "canvas_Persistency_WrappedStdDictionaries",
        "art_Persistency_WrappedStdDictionaries_dict" => "canvas_Persistency_WrappedStdDictionaries_dict",
        "art_Utilities" => "art_Utilities canvas"
                       ); }

foreach my $lib (sort keys %dir_list) {
   next if m&art_Persistency_Common canvas&i;
   next if m&art_Persistency_Provenance canvas&i;
   next if m&art_Utilities canvas&i;
  #s&\b\Q${lib}\E([^\.\s]*\b)([^\.]|$)&$dir_list{$lib}${1}${2}&g and last;
  s&\b\Q${lib}\E\b([^\.]|$)&$dir_list{$lib}${1}${2}&g and last;
}

s/\$\{ART_PERSISTENCY_COMMON\}/art_Persistency_Common canvas/g;
s/\$\{ART_PERSISTENCY_PROVENANCE\}/art_Persistency_Provenance canvas/g;
s/\$\{ART_UTILITIES\}/art_Utilities canvas/g;
s&\$ENV\{ART_DIR\}/Modules&\$ENV\{CANVAS_DIR\}/Modules&g;
