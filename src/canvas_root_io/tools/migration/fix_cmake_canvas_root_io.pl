# Precondition: only files that need to have the
# 'find_ups_product(canvas_root_io)' added will be presented.

use strict;

s&\$ENV\{CANVAS_DIR\}/Modules&\$ENV\{CANVAS_ROOT_IO_DIR\}/Modules&g;
if (m&find_ups_product\(\s*canvas(:?\s+v\d_\d{2}_\d{2})?\s*\)&) {
    print "find_ups_product(canvas_root_io)\n";
}
