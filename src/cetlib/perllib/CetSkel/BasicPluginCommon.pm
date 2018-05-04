use strict;

package CetSkel::BasicPluginCommon;

use Exporter;

use vars qw(@ISA @EXPORT @EXPORT_OK);

@ISA = qw(Exporter);
@EXPORT = qw($bp_headers);
@EXPORT_OK = qw(&pluginSuffix);

sub pluginSuffix {
  return "_plugin";
}

1;

### Local Variables:
### mode: cperl
### End:
