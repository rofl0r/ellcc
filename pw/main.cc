#include "pwPlexer.h"

static pw::ErrorList errors;

//
int main(int argc, char** argv)
{
    std::string file(argv[1]);
    pw::PP* pp = new pw::PP(file, errors);
    FILE* fp = NULL;
    if (pp == NULL) exit(1);
    if (!pp->setInput(fp)) {
        fprintf(stderr, "can't open %s\n", argv[1]);
        delete pp;
        exit(1);
    }
    
#if 0
    // Define any predefined macros.
    for (int index = 0; index < lp->macros.size(); ++index) {
        pp->addDefine(lp->macros[index].name, lp->macros[index].definition);
    }
#endif

    const pw::Plexer* language = pw::Plexer::Create("c99.cfg", errors);
    if (language) {
        pw::Options options = language->options;
        pp->setOptions(&options);    		// Set pre-processor options.
        pp->addDefine("__i386__");							// RICH
        pp->addInclude("/usr/lib/gcc/i686-pc-cygwin/3.4.4/include");			// RICH
        pp->addInclude("/usr/lib/gcc-lib/i386-redhat-linux/3.3.2/include");		// RICH
        pp->addInclude("/usr/lib/gcc/i386-redhat-linux/4.1.2/include");		        // RICH
        pp->addInclude("/usr/include");							// RICH

        pp->getToken(pw::PP::GETALL);
        errors.file = pp->info.file;
        std::string lastfile;
        for (;;) {
            if (pp->info.token == pw::PPStream::ENDOFFILE) {
                // End of file.
                break;
            }
            if (errors.file != lastfile) {
                // Output #line directive if pre-processing.
                lastfile = errors.file;
                fprintf(stdout, "#line %d \"%s\"\n", pp->info.startline, errors.file.c_str());
            }
            fprintf(stdout, "%s", pp->info.string.c_str());
            pp->getToken(pw::PP::GETALL);
            errors.file = pp->info.file;     	// Remember the last file for error reporting.
        }
    }

    int totalerrors = 0;
    for (int j = 0; j < pw::Error::ERRORCNT; ++j) {
        // Calculate the total number of errors.
        totalerrors += errors.errorCount(j);
    }

    if (totalerrors) {
        for (int i = 0; i < pw::Error::ERRORCNT; ++i) {
            const char *name;
            const char *plural;
            int count = errors.errorCount(i);

            if (count == 0)
                continue;

            if (count == 1)
                plural = "";
            else
                plural = "s";

            name = pw::Error::modifier((pw::Error::Type)i);
            fprintf(stdout, "%d %s message%s reported\n", count, name, plural);
        }

        // Show errors.
        errors.sort();
        errors.print(stdout);
    }
}
