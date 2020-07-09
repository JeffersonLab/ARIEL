use strict;

package CetSkelPlugins::ELdestination;

# Not currently useful (2015/04/17), since we provide our own version of
# the only function it provides for us.
#  use CetSkel::BasicPluginCommon;

use vars qw(@ISA);

eval "use CetSkelPlugins::messagefacility::PluginVersionInfo";
unless ($@) {
  push @ISA, "CetSkelPlugins::messagefacility::PluginVersionInfo";
}

sub new {
  my $class = shift;
  my $self = { };
  return bless \$self, $class;
}

sub type { return "eldestination"; }

sub source { return __FILE__; }

sub baseClasses {
  return
    [
     { header => 'messagefacility/MessageLogger/ELdestination.h',
       class => "mf::service::ELdestination",
       protection => "public" # Default.
     }
    ];
}

sub constructors {
  return [ {
            explicit => 1,
            args => [ "fhicl::ParameterSet const& p" ],
            initializers => [ "ELdestination(p)" ]
           } ];
}

sub defineMacro {
  my ($self, $qual_name) = @_;
  return <<EOF;
MAKE_PLUGIN_START(auto,
                  std::string const& psetName,
                  fhicl::ParameterSet const& pset)
{
  return std::make_unique<${qual_name}>( /* args as necessary. */ );
}
MAKE_PLUGIN_END

CET_PROVIDE_FILE_PATH()
DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)
FHICL_PROVIDE_ALLOWED_CONFIGURATION(${qual_name})
EOF
}

# No sub implHeaders necessary.

sub implHeaders {
  return [ '"cetlib/PluginTypeDeducer.h"',
           '"cetlib/ProvideFilePathMacro.h"',
           '"cetlib/ProvideMakePluginMacros.h"',
           '"fhiclcpp/ParameterSet.h"',
           '"fhiclcpp/types/AllowedConfigurationMacro.h"' ]
}

# No sub macrosInclude necessary.

sub optionalEntries {
  return
    {
     finish => 'void finish() override',
     log => 'void log(mf::ErrorObj& msg) override',
     summarization => 'void summarization(mf::ELstring const& title, mf::ELstring const& sumLines) override',
     summary => 'void summary() override',
     wipe => 'void wipe() override',
    };
}

sub pluginSuffix {
  return "_mfplugin";
}

# No sub requiredEntries necessary.

1;
