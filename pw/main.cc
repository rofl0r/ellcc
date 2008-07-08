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
#include "pwOS.h"

/* The list of errors that occur during preprocessing.
 */
static pw::ErrorList errors;

int main(int argc, char** argv)
{
    pw::array<char*> includes;
    
    // Simple command line argument gathering.
    for (int i = 1; i < argc; ++i) {
        if (*argv[i] == '-') {
            if (*(argv[i] + 1) == 'I') {
                // Add an include path.
                includes += argv[i] + 2;
            }

            argv[i] = NULL;
        }
    }

    // Find configuration files.
    std::string exe = pw::findExecutable(argc, argv);
    std::string path;
    std::string mypath;
    std::string name;
    std::string extension;
    pw::parseFilename(exe, path, name, extension);
    std::string config = pw::buildFilename("", name, "ecf");

    // Set up configuration file include paths.
    pw::Plexer* configuration = pw::Plexer::Create("ecf", errors);
    if (configuration == NULL) {
        goto showerrors;
    }
    configuration->addInclude("");                 // Add the current directory.
    mypath = pw::buildFilename(path, "..");
    mypath = pw::buildFilename(mypath, "config");
    configuration->addInclude(mypath.c_str());     // Add <exe dir>../config.
    mypath = pw::buildFilename(path, "..");
    mypath = pw::buildFilename(mypath, "lib");
    mypath = pw::buildFilename(mypath, "pwpp");
    configuration->addInclude(mypath.c_str());     // Add <exe dir>../lib/pwpp.
    // Read the program configuration file.
    configuration = pw::Plexer::Create(config.c_str(), errors);
    if (configuration == NULL) {
        goto showerrors;
    }

    for (int i = 1; i < argc; ++i) {
        if (argv[i] == NULL) {
            // The argument was already processed.
            continue;
        }

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
    
        // Set up the output file.
        FILE* ofp;
        std::string path;
        std::string name;
        std::string extension;
        pw::parseFilename(file, path, name, extension);
        name = pw::buildFilename("", name, "i");
        ofp = pw::tfopen(name.c_str(), "w");
        if (ofp == NULL) {
            fprintf(stderr, "can't open %s for writing\n", name.c_str());
            delete pp;
            exit(1);
        }

        if (configuration) {
            pw::Options options = configuration->options;
            pp->setOptions(&options);    		// Set pre-processor options.

            // Set pre-defined macros.
            for (int i = 0; i < configuration->macros.size(); ++i) {
                pp->addDefine(configuration->macros[i]);
            }

            // Set the user include paths.
            for (int i = 0; i < includes.size(); ++i) {
                pp->addUserInclude(includes[i]);
            }

            // Set the include paths.
            for (int i = 0; i < configuration->includes.size(); ++i) {
                pp->addInclude(configuration->includes[i]);
            }

            // Preprocess the file.
            const char* lastfile;
            lastfile = errors.file;
            
            fprintf(ofp, "# %d \"%s\"\n", 1, errors.file);
            pp->getToken(pw::PP::GETALL);
            for (;;) {
                if (pp->info.token == pw::PPStream::ENDOFFILE) {
                    // End of file.
                    break;
                }

                if (errors.file != lastfile) {
                    // Output #line directive if pre-processing.
                    lastfile = errors.file;
                    fprintf(ofp, "# %d \"%s\"\n", pp->info.startline, errors.file);
                }

                fprintf(ofp, "%s", pp->info.string.c_str());
                pp->getToken(pw::PP::GETALL);
            }
        }

        pw::fclose(ofp);
        delete pp;
    }

showerrors:
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

    exit(errors.hasErrors() ? 1 : 0);
}
