#!/usr/bin/perl -w
# configure script for libbasic

use strict 'subs';

# default location of smbase relative to this package
$SMBASE = "../SMBase";
$req_smcv = 1.03;            # required sm_config version number
$thisPackage = "libbasic";

# -------------- BEGIN common block ---------------
# do an initial argument scan to find if smbase is somewhere else
for (my $i=0; $i < @ARGV; $i++) {
  my ($d) = ($ARGV[$i] =~ m/-*smbase=(.*)/);
  if (defined($d)) {
    $SMBASE = $d;
  }
}

# try to load the sm_config module
eval {
  push @INC, ($SMBASE);
  require sm_config;
};
if ($@) {
  die("$@" .     # ends with newline, usually
      "\n" .
      "I looked for smbase in \"$SMBASE\".\n" .
      "\n" .
      "You can explicitly specify the location of smbase with the -smbase=<dir>\n" .
      "command-line argument.\n");
}

# check version number
$smcv = get_sm_config_version();
if ($smcv < $req_smcv) {
  die("This package requires version $req_smcv of sm_config, but found\n" .
      "only version $smcv.\n");
}
# -------------- END common block ---------------

# defaults
@LDFLAGS = ("-g -Wall");
$LLVM = "/usr/local";
$GCOV_MODS = "";


sub usage {
  standardUsage();

  print(<<"EOF");
package options:
  -prof              enable profiling
  -gcov=<mods>       enable coverage testing for modules <mods>
  -devel             add options useful while developing (-Werror)
  -llvm=<dir>:       specify where the LLVM system is [$LLVM]
EOF
}


# -------------- BEGIN common block 2 -------------
# global variables holding information about the current command-line
# option being processed
$option = "";
$value = "";

# process command-line arguments
foreach $optionAndValue (@ARGV) {
  # ignore leading '-' characters, and split at first '=' (if any)
  ($option, $value) =
    ($optionAndValue =~ m/^-*([^-][^=]*)=?(.*)$/);
                      #      option     = value

  my $arg = $option;

  if (handleStandardOption()) {
    # handled by sm_config.pm
  }
  # -------------- END common block 2 -------------

  elsif ($arg eq "prof") {
    push @CCFLAGS, "-pg";
    push @LDFLAGS, "-pg";
  }

  elsif ($arg eq "gcov") {
    $GCOV_MODS = getOptArg();
  }

  elsif ($arg eq "devel") {
    push @CCFLAGS, "-Werror";
  }

  elsif ($arg eq "llvm") {
    $LLVM = getOptArg();
  }

  else {
    die "unknown option: $arg\n";
  }
}

finishedOptionProcessing();


# ------------------ check for needed components ----------------
test_smbase_presence();

test_CXX_compiler();

# LLVM
if (! -d "$LLVM/include/llvm") {
  die "Cannot find $LLVM/include/llvm.\n" .
      "The LLVM system is required for libbasic.\n" .
      "If it's in a different location, use the -llvm=<dir> option.\n";
}

$PERL = get_PERL_variable();


# ------------------ config.summary -----------------
$summary = getStandardConfigSummary();

$summary .= <<"OUTER_EOF";
cat <<EOF
  LDFLAGS:     @LDFLAGS
  SMBASE:      $SMBASE
  LLVM:        $LLVM
EOF
OUTER_EOF

if ($GCOV_MODS) {
  $summary .= "echo \"  GCOV_MODS:   $GCOV_MODS\"\n";
}

writeConfigSummary($summary);


# ------------------- config.status ------------------
writeConfigStatus("LDFLAGS" => "@LDFLAGS",
                  "SMBASE" => "$SMBASE",
                  "PERL" => "$PERL",
                  "LLVM" => "$LLVM");


# ----------------- final actions -----------------
# run the output file generator
run("./config.status");

print("\nYou can now run make, usually called 'make' or 'gmake'.\n");

exit(0);


# silence warnings
pretendUsed($thisPackage);
