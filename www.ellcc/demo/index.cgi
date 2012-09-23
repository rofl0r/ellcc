#!/usr/bin/perl -w
# ELLCC Web Demo script
#

use strict;
use CGI;
use POSIX;
use Mail::Send;

$| = 1;

my $ROOT = "/tmp/webcompile";

open( STDERR, ">&STDOUT" ) or die "can't redirect stderr to stdout";

if ( !-d $ROOT ) { mkdir( $ROOT, 0777 ); }

my $LOGFILE         = "$ROOT/log.txt";
my $FORM_URL        = 'index.cgi';
my $MAILADDR        = 'rich@npennware.com';
my $CONTACT_ADDRESS = 'Questions or comments? Email <a href="mailto:rich@pennware.com">me</a>.';
my $LOGO_IMAGE_URL  = '../ellcc.png';
my $TIMEOUTAMOUNT   = 20;

my @PREPENDPATHDIRS =
    ('/home/rich/ellcc/bin');

my $defaultsrc = "/*\n" .
" *      Erastosthenes Sieve Prime Number Program in C\n" .
" *      from Byte, Sept. 1981, pg. 186\n" .
" */\n" .
"\n" .
"#include <stdio.h>\n" .
"\n" .
"#define TRUE    1\n" .
"#define FALSE   0\n" .
"#define SIZE    8190\n" .
"#define SIZEPL  8191\n" .
"\n" .
"char    flags[SIZEPL];\n" .
"\n" .
"int main(int ac, char **av)\n" .
"{\n" .
"	 int     i,prime,k,count,iter;\n" .
"\n" .
'	 printf("10000 iterations\n");' .
"\n        for(iter=1; iter <= 10000; iter++)\n" .
"                {\n" .
"                count=0;\n" .
"                for(i=0; i <= SIZE; i++)\n" .
"                        flags[i]=TRUE;\n" .
"\n" .
"                for(i=0; i <= SIZE; i++)\n" .
"                        {\n" .
"                        if(flags[i])\n" .
"                                {\n" .
"                                prime=i+i+3;\n" .
"                                k=i+prime;\n" .
"                                while(k <= SIZE)\n" .
"                                        {\n" .
"                                        flags[k]=FALSE;\n" .
"                                        k+=prime;\n" .
"                                        }\n" .
"                                count=count+1;\n" .
"                                }\n" .
"                        }\n" .
"                }\n" .
'        printf("%d primes\n",count);' .
"\n}\n";
sub getname {
    my ($extension) = @_;
    for ( my $count = 0 ; ; $count++ ) {
        my $name =
          sprintf( "$ROOT/_%d_%d%s", $$, $count, $extension );
        if ( !-f $name ) { return $name; }
    }
}

my $c;

sub barf {
    print "<b>", @_, "</b>\n";
    print $c->end_html;
    system("rm -f $ROOT/locked");
    exit 1;
}

sub writeIntoFile {
    my $extension = shift @_;
    my $contents  = join "", @_;
    my $name      = getname($extension);
    local (*FILE);
    open( FILE, ">$name" ) or barf("Can't write to $name: $!");
    print FILE $contents;
    close FILE;
    return $name;
}

sub addlog {
    my ( $source, $pid, $result ) = @_;
    open( LOG, ">>$LOGFILE" );
    my $time       = scalar localtime;
    my $remotehost = $ENV{'REMOTE_ADDR'};
    print LOG "[$time] [$remotehost]: $pid\n";
    print LOG "<<<\n$source\n>>>\nResult is: <<<\n$result\n>>>\n";
    close LOG;
}

sub syntaxHighlightConsoleOutput {
  my ($input) = @_;
  $input =~ s@\033\[(?:\d;)?(\d);?((?:\d\d)?)m@</span><span class="terminalStyle$1$2">@g;
  return $input;
}

sub dumpFile {
    my ( $header, $file ) = @_;
    my $result;
    open( FILE, "$file" ) or barf("Can't read $file: $!");
    while (<FILE>) {
        $result .= $_;
    }
    close FILE;
    my $UnhilightedResult = $result;
    my $HtmlResult        =
      "<h3>$header</h3>\n<pre>\n<span>" . syntaxHighlightConsoleOutput($c->escapeHTML($result)) . "</span>\n</pre>\n";
    if (wantarray) {
        return ( $UnhilightedResult, $HtmlResult );
    }
    else {
        return $HtmlResult;
    }
}

sub syntaxHighlightLLVM {
  my ($input) = @_;
  $input =~ s@\b(void|i\d+|float|double|x86_fp80|fp128|ppc_fp128|type|label|opaque)\b@<span class="llvm_type">$1</span>@g;
  $input =~ s@\b(ret|br|switch|indirectbr|invoke|unwind|unreachable|add|sub|mul|udiv|sdiv|fdiv|urem|srem|frem|shl|lshr|ashr|and|or|xor|extractelement|insertelement|shufflevector|extractvalue|insertvalue|malloc|free|alloca|load|store|getelementptr|trunc|zext|sext|fptrunc|fpext|fptoui|fptosi|uitofp|sitofp|ptrtoint|inttoptr|bitcast|to|blockaddress|icmp|fcmp|phi|select|call|va_arg|eq|ne|ugt|uge|ult|ule|sgt|sge|slt|sle|oeq|ogt|oge|olt|ole|one|ord|ueq|une|uno|tail|begin|end|true|false|declare|global|constant|const|private|internal|linkonce|common|weak|appending|extern_weak|dllimport|dllexport|ccc|fastcc|coldcc|uninitialized|external|implementation|linkonce|weak|appending|null|except|not|target|endian|pointersize|big|little|volatile|zeroinitializer|define|protected|hidden|addrspace|section|align|alias|signext|zeroext|inreg|byval|sret|noalias|nocapture|nest|gc|alwaysinline|noinline|optsize|noreturn|nounwind|readnone|readonly|ssp|sspreq|module|asm|sideeffect)\b@<span class="llvm_keyword">$1</span>@g;

  # Add links to the FAQ.
  $input =~ s@(_ZNSt8ios_base4Init[DC]1Ev)@<a href="../docs/FAQ.html#iosinit">$1</a>@g;
  $input =~ s@\bundef\b@<a href="../docs/FAQ.html#undef">undef</a>@g;
  return $input;
}

sub mailto {
    my ( $recipient, $body ) = @_;
    my $msg =
      new Mail::Send( Subject => "ELLCC Demo Page Run", To => $recipient );
    my $fh = $msg->open();
    print $fh $body;
    $fh->close();
}

$c = new CGI;
print $c->header;

print <<EOF;
<html>
<head>
  <meta content="text/html; charset=ISO-8859-1"
 http-equiv="Content-Type">
  <title>Try out ELLCC in your browser!</title>
  <meta content="Richard Pennington" name="author">
  <style>
    \@import url("syntax.css");
  </style>
</head>
<body>
<table style="text-align: left; width: 100%; height: 100%;" border="0"
 cellpadding="0" cellspacing="0">
  <tbody>
    <tr align="center">
      <th
 style="text-align: center; vertical-align: middle; background-color: rgb(192, 192, 192); height: 135px;"
 colspan="2" rowspan="1">
      <table style="text-align: left; width: 100%;" border="0"
 cellpadding="0" cellspacing="0">
        <tbody>
          <tr>
            <td style="height: 135px; width: 135px;"><img
 style="width: 135px; height: 135px;" alt="ELLCC" src="../images/ellcc.png"></td>
            <td style="text-align: center;">
            <h1>Try out ELLCC in your browser!</h1>
            </td>
          </tr>
        </tbody>
      </table>
      </th>
    </tr>
    <tr>
      <td
 style="vertical-align: top; background-color: rgb(153, 255, 153); width: 135px;"><a
 href="../index.html">Home</a><br>
      <a href="../blog/">Blog</a><br>
      <a href="../news.html">News</a><br>
      <a href="../demo/">Demo</a><br>
      <a href="../ellcc/libecc/share/doc">Documentation</a><br>
      <a href="../installation.html">Installation</a><br>
      <a href="../targetsupport.html">Target Support</a><br>
      <a href="../bugzilla">Bug Database</a><br>
      <a href="http://ellcc.org/viewvc/svn/ellcc/">Source Repository</a><br>
      <a href="mailto:rich\@pennware.com">Contact</a><br>
      <br>
The ELLCC project is supported by <a href="http://pennware.com">Pennington
Software</a><br>
      <br>
      </td>
      <td style="vertical-align: top;">
      <table>
EOF

if ( -f "$ROOT/locked" ) {
  my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$locktime) = 
    stat("$ROOT/locked");
  my $currtime = time();
  if ($locktime + 60 > $currtime) {
    print "This page is already in use by someone else at this ";
    print "time, try reloading in a second or two.</td></tr></table>'\n";
    exit 0;
  }
}

system("touch $ROOT/locked");

print <<END;
Paste a C/C++ program in the text box or
upload one from your computer, and you can see ELLCC compile it!
This demo is very similar to the <a href="http://llvm.org/demo/">LLVM demo page</a> but
allows you to compile for a wider set of processors. The test program is a little bit of a nostalgic joke.
It was a popular program for benchmarking embedded C compilers back then. It's even a little more funny since
my browser has told me I misspelled Eratosthenes back then and never noticed.
</td></tr></table><p>
END

my %llvmTargets = ();
$llvmTargets{'microblaze'} = { label => '32-bit Microblaze soft core'  };
$llvmTargets{'mips'} = { label => '32-bit Mips'  };
$llvmTargets{'mips32r2'} = { label => '32-bit Mips r2'  };
$llvmTargets{'mips32r2sf'} = { label => '32-bit Mips r2 Soft Float'  };
$llvmTargets{'arm'} = { label => '32-bit ARM'  };
$llvmTargets{'armv8'} = { label => '32-bit ARMv8'  };
$llvmTargets{'ppc'} = { label => '32-bit PowerPC'  };
$llvmTargets{'ppc64'} = { label => '64-bit PowerPC'  };
$llvmTargets{'i386'} = { label => '32-bit X86: Pentium-Pro and above'  };
$llvmTargets{'x86_64'}  = { label => '64-bit X86: EM64T and AMD64' };
$llvmTargets{'llvm'} = { label => 'LLVM assembly' };
my %targetLabels = map { $_ => $llvmTargets{$_}->{'label'} } keys %llvmTargets;
sub llvmTargetsSortedByLabel {
  $llvmTargets{$a}->{'label'} cmp $llvmTargets{$b}->{'label'};
}

my @sortedTargets = sort llvmTargetsSortedByLabel keys %llvmTargets;

print $c->start_multipart_form( 'POST', $FORM_URL );

my $source = $c->param('source');


# Start the user out with something valid if no code.
$source = $defaultsrc if (!defined($source));

print '<table border="0"><tr><td>';

print "Type your source code in below: (<a href='DemoInfo.html#hints'>hints and 
advice</a>)<br>\n";

print $c->textarea(
    -name    => "source",
    -rows    => 16,
    -columns => 60,
    -default => $source
), "<br>";

print "Or upload a file: ";
print $c->filefield( -name => 'uploaded_file', -default => '' );

print "<p />\n";


print '<p></td><td valign=top>';

print "<center><h3>General Options</h3></center>";

print "Source language: ",
  $c->radio_group(
    -name    => 'language',
    -values  => [ 'C', 'C++' ],
    -default => 'C'
  ), "<p>";

print "Optimization level: ",
  $c->radio_group(
    -name    => 'optlevel',
    -values  => [ 'Standard', 'None' ],
    -default => 'Standard'
  ),' <a href="DemoInfo.html#optlevel">?</a><br>', "<p>";

print $c->checkbox(
    -name  => 'showstats',
    -label => 'Show detailed pass statistics'
  ), ' <a href="DemoInfo.html#stats">?</a><br>';

print $c->checkbox(
    -name  => 'cxxdemangle',
    -label => 'Demangle C++ names'
  ),' <a href="DemoInfo.html#demangle">?</a><p>';


print "<center><h3>Output Options</h3></center>";

print "Target: ",
  $c->popup_menu(
    -name    => 'target',
    -default => 'llvm',
    -values => \@sortedTargets,
    -labels => \%targetLabels
  ), ' <a href="DemoInfo.html#target">?</a><p>';

print $c->checkbox(
    -name => 'showbcanalysis',
    -label => 'Show detailed bytecode analysis'
  ),' <a href="DemoInfo.html#bcanalyzer">?</a><br>';

print "</td></tr></table>";

print "<center>", $c->submit(-value=> 'Compile Source Code'), 
      "</center>\n", $c->endform;

print "\n<p>If you have questions about the LLVM code generated by the
front-end, please check the <a href='../ellcc/docs/llvm/html/FAQ.html#cfe_code'>FAQ</a> and
the demo page <a href='DemoInfo.html#hints'>hints section</a>.
</p>\n";

$ENV{'PATH'} = ( join ( ':', @PREPENDPATHDIRS ) ) . ":" . $ENV{'PATH'};

sub sanitychecktools {
    my $sanitycheckfail = '';

    # insert tool-specific sanity checks here
    $sanitycheckfail .= ' llvm-dis'
      if `llvm-dis --help 2>&1` !~ /ll disassembler/;

    $sanitycheckfail .= ' ecc'
      if `ecc --help 2>&1` !~ /clang LLVM compiler/;

    $sanitycheckfail .= ' llc'
      if `llc --help 2>&1` !~ /llvm system compiler/;

    $sanitycheckfail .= ' llvm-bcanalyzer'
      if `llvm-bcanalyzer --help 2>&1` !~ /bcanalyzer/;
    $sanitycheckfail .= ' ecc'
      if `ecc --version 2>&1` !~ /clang/;

    barf(
"<br/>The demo page is currently unavailable. [tools: ($sanitycheckfail ) failed sanity check]"
      )
      if $sanitycheckfail;
}

sanitychecktools();

sub try_run {
    my ( $program, $commandline, $outputFile ) = @_;
    my $retcode = 0;

    eval {
        local $SIG{ALRM} = sub { die "timeout"; };
        alarm $TIMEOUTAMOUNT;
        $retcode = system($commandline);
        alarm 0;
    };
    if ( $@ and $@ =~ /timeout/ ) { 
      barf("Program $program took too long, compile time limited for the web script, sorry!.\n"); 
    }
    if ( -s $outputFile ) {
        print scalar dumpFile( "Output from $program", $outputFile );
    }
    #print "<p>Finished dumping command output.</p>\n";
    if ( WIFEXITED($retcode) && WEXITSTATUS($retcode) != 0 ) {
        barf(
"$program exited with an error. Please correct source and resubmit.<p>\n" .
"Please note that this form only allows fully formed and correct source" .
" files.  It will not compile fragments of code.<p>"
        );
    }
    if ( WIFSIGNALED($retcode) != 0 ) {
        my $sig = WTERMSIG($retcode);
        barf(
            "Ouch, $program caught signal $sig. Sorry, better luck next time!\n"
        );
    }
}

my %suffixes = (
    'C'                => '.c',
    'C++'              => '.cc',
    'Objective-C'      => '.m',
    'Objective-C++'    => '.mm',
    'preprocessed C'   => '.i',
    'preprocessed C++' => '.ii'
);
my %languages = (
    '.c'    => 'C',
    '.i'    => 'preprocessed C',
    '.ii'   => 'preprocessed C++',
    '.cc'   => 'C++',
    '.cpp'  => 'C++',
);
my %language_options = (
    'C'                => '',
    'C++'              => '',
    'preprocessed C'   => '',
    'preprocessed C++' => ''
);

my $uploaded_file_name = $c->param('uploaded_file');
if ($uploaded_file_name) {
    if ($source) {
        $source = "";
    }
    $uploaded_file_name =~ s/^.*(\.[A-Za-z0-9]+)$/$1/;
    my $language = $languages{$uploaded_file_name};
    $c->param( 'language', $language );

    print "<p>Processing uploaded file. It looks like $language.</p>\n";
    my $fh = $c->upload('uploaded_file');
    if ( !$fh ) {
        barf( "Error uploading file: " . $c->cgi_error );
    }
    while (<$fh>) {
        $source .= $_;
    }
    close $fh;
}

if ($c->param && $source) {
    # Since we inject target name in command line tool (llc), we need to
    # validate it properly. Check if chosen target is an known valid target.
    my $target = $c->param('target');
    my $targetHTML = $c->escapeHTML($target);
    barf(
      "Unknown target $targetHTML. Please choose another one."
    ) unless exists $llvmTargets{$target};

    print $c->hr;
    my $extension = $suffixes{ $c->param('language') };
    barf "Unknown language; can't compile\n" unless $extension;

    # Add a newline to the source here to avoid a warning from gcc.
    $source .= "\n";

    # Avoid security hole due to #including bad stuff.
    $source =~
s@(\n)?#include.*[<"](.*\.\..*)[">].*\n@$1#error "invalid #include file $2 detected"\n@g;

    my $inputFile = writeIntoFile( $extension, $source );
    my $pid       = $$;

    my $bytecodeFile = getname(".bc");
    my $outputFile   = getname(".clang.out");
    my $timerFile    = getname(".clang.time");

    my $stats = '';
    #$stats = "-Wa,--stats,--time-passes,--info-output-file=$timerFile"
    $stats = "-ftime-report"
	if ( $c->param('showstats') );

    my $options = $language_options{ $c->param('language') };
    $options .= " -O3" if $c->param('optlevel') ne "None";

    try_run( "llvm C/C++ front-end (ecc)",
	"ecc -fcolor-diagnostics -emit-llvm -msse3 -W -Wall $options $stats -o $bytecodeFile -c $inputFile > $outputFile 2>&1",
      $outputFile );

    if ( $c->param('showstats') && -s $timerFile ) {
        my ( $UnhilightedResult, $HtmlResult ) =
          dumpFile( "Statistics for front-end compilation", $timerFile );
        print "$HtmlResult\n";
    }

    print " Bytecode size is ", -s $bytecodeFile, " bytes.\n";

    #my $target = $c->param('target');
    my $targetLabel = $llvmTargets{$target}->{'label'};

    my $disassemblyFile;
    if ( $target eq 'llvm' ) {
        $disassemblyFile = getname(".ll");
        try_run( "llvm-dis",
            "llvm-dis -o=$disassemblyFile $bytecodeFile > $outputFile 2>&1",
            $outputFile );
    } else {
        $disassemblyFile = getname(".s");
        my $options = ( $c->param('optlevel') eq "None" ) ? "-O0" : "-O3";
        try_run( "$target-ellcc-linux",
            "ecc -target $target-ellcc-linux -S $options -o $disassemblyFile $inputFile > $outputFile 2>&1",
            $outputFile );
    }

    if ( $c->param('cxxdemangle') ) {
        print " Demangling target output.\n";
        my $tmpFile = getname(".ll");
        system("c++filt < $disassemblyFile > $tmpFile 2>&1");
        system("mv $tmpFile $disassemblyFile");
    }

    my ( $UnhilightedResult, $HtmlResult );
    if ( -s $disassemblyFile ) {
        my $programName = ( $target eq 'llvm' ) ? 'disassembler' : 'compiler';
        ( $UnhilightedResult, $HtmlResult ) =
          dumpFile( "Output from the $programName targeting $targetLabel", $disassemblyFile );
        if ( $target eq 'llvm' ) {
            $HtmlResult = syntaxHighlightLLVM($HtmlResult);
        }
        # It would be nice to support highlighting of other assembly files.
        print $HtmlResult;
    }
    else {
        print "<p>Hmm, that's weird, llvm-dis/llc didn't produce any output.</p>\n";
    }

    if ( $c->param('showbcanalysis') ) {
      my $analFile = getname(".bca");
      try_run( "llvm-bcanalyzer", "llvm-bcanalyzer $bytecodeFile > $analFile 2>&1", 
        $analFile);
    }

    # Get the source presented by the user to CGI, convert newline sequences to simple \n.
    my $actualsrc = $c->param('source');
    $actualsrc =~ s/\015\012/\n/go;
    # Don't log this or mail it if it is the default code.
    if ($actualsrc ne $defaultsrc) {
    addlog( $source, $pid, $UnhilightedResult );

    my ( $ip, $host, $lg, $lines );
    chomp( $lines = `wc -l < $inputFile` );
    $lg = $c->param('language');
    $ip = $c->remote_addr();
    chomp( $host = `host $ip` ) if $ip;
    if (0) {  # MAILING IS DISABLED
      mailto( $MAILADDR,
        "--- Query: ---\nFrom: ($ip) $host\nInput: $lines lines of $lg\n"
          . "C++ demangle = "
          . ( $c->param('cxxdemangle') ? 1 : 0 )
          . ", Opt level = "
          . ( $c->param('optlevel') ) . "\n\n"
          . ", Show stats = "
          . ( $c->param('showstats') ? 1 : 0 ) . "\n\n"
          . "--- Source: ---\n$source\n"
          . "--- Result: ---\n$UnhilightedResult\n" );
      }
    }
    unlink( $inputFile, $bytecodeFile, $outputFile, $disassemblyFile );
}

print <<END;
</td></tr></tbody></table></body>
END
print $c->hr, $c->end_html;
system("rm $ROOT/locked");
exit 0;
