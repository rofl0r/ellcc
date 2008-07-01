/** @file
 * A simple preprocessor.
 * @author Richard Pennington
 * @date July 1, 2008
 *
 * Copyright (C) 2008, Richard Pennington.
 *
 * This file is a simple driver for the pw Lexer.
 * It is not meant to be a full cpp replacement but merely an example of
 * how one could use the pw lexer.
 */
#include "pwPlexer.h"

/* The list of errors that occur during preprocessing.
 */
static pw::ErrorList errors;

int main(int argc, char** argv)
{
    // Simple command line argument gathering.

    for (int i = 1; i < argc; ++i) {
        if (*argv[i] == '-') {
        }
    }

    for (int i = 1; i < argc; ++i) {
        if (argv[i] == NULL) {
            // The argument was already processed.
            continue;
        }

        const pw::Plexer* language = pw::Plexer::Create("c99.cfg", errors);

        std::string file(argv[i]);
        pw::PP* pp = new pw::PP(file, errors);
        FILE* fp = NULL;

        if (pp == NULL) {
            // An error occured creating the preprocessor.
            exit(1);
        }

        if (!pp->setInput(fp)) {
            fprintf(stderr, "can't open %s\n", file.c_str());
            delete pp;
            exit(1);
        }
    
        if (language) {
            pw::Options options = language->options;
            pp->setOptions(&options);    		// Set pre-processor options.

            // Set pre-defined macros.
            for (int i = 0; i < language->macros.size(); ++i) {
                pp->addDefine(language->macros[i]);
            }

            // Set the include paths.
            for (int i = 0; i < language->includes.size(); ++i) {
                pp->addInclude(language->includes[i]);
            }

            // Preprocess the file.
            const char* lastfile;
            lastfile = errors.file;
            fprintf(stdout, "# %d \"%s\"\n", 1, errors.file);
            pp->getToken(pw::PP::GETALL);
            for (;;) {
                if (pp->info.token == pw::PPStream::ENDOFFILE) {
                    // End of file.
                    break;
                }

                if (errors.file != lastfile) {
                    // Output #line directive if pre-processing.
                    lastfile = errors.file;
                    fprintf(stdout, "# %d \"%s\"\n", pp->info.startline, errors.file);
                }

                fprintf(stdout, "%s", pp->info.string.c_str());
                pp->getToken(pw::PP::GETALL);
            }
        }

        delete pp;
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
