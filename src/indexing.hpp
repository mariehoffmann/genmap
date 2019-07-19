#pragma once

#include <type_traits>
#include <sys/types.h>
#include <dirent.h>

#include <vector>
#include <cstdint>
#include <limits>
#include <cmath>
#include <sys/stat.h>

#include <seqan/arg_parse.h>
#include <seqan/seq_io.h>
#include <seqan/index.h>

#include "common.hpp"

#include "../include/lambda/src/mkindex_saca.hpp"
#include "../include/lambda/src/mkindex_misc.hpp"
#include "../include/lambda/src/mkindex_algo.hpp"

using namespace seqan;

static constexpr bool outputProgress = true; // TODO: remove global variable

#include "common.hpp"
#include "algo.hpp"
#include "output.hpp"

/*
template <typename TVector, typename TChromosomeNames, typename TChromosomeLengths, typename TLocations, typename TDirectoryInformation>
inline void outputMappability(TVector const & c, Options const & opt, SearchParams const & searchParams,
                              std::string const & fastaFile, TChromosomeNames const & chromNames,
                              TChromosomeLengths const & chromLengths, TLocations & locations,
                              TDirectoryInformation const & directoryInformation)
{
    std::cout << "Start writing output files ...";
    if (opt.verbose)
        std::cout << '\n' << std::flush;

    std::string output_path = std::string(toCString(opt.outputPath));
    output_path += fastaFile.substr(0, fastaFile.find_last_of('.')) + ".genmap";

    if (opt.rawFile)
    {
        double start = get_wall_time();
        if (opt.outputType == OutputType::mappability)
            saveRaw<true>(c, output_path + ".map");
        else if (opt.outputType == OutputType::frequency_small)
            saveRaw<false>(c, output_path + ".freq8");
        else // if (opt.outputType == OutputType::frequency_large)
            saveRaw<false>(c, output_path + ".freq16");
        if (opt.verbose)
            std::cout << "- RAW file written in " << (round((get_wall_time() - start) * 100.0) / 100.0) << " seconds\n";
    }

    if (opt.txtFile)
    {
        double start = get_wall_time();
        if (opt.outputType == OutputType::mappability)
            saveTxt<true>(c, output_path, chromNames, chromLengths);
        if (opt.outputType == OutputType::frequency_small || opt.outputType == OutputType::frequency_large)
            saveTxt<false>(c, output_path, chromNames, chromLengths);
        if (opt.verbose)
            std::cout << "- TXT file written in " << (round((get_wall_time() - start) * 100.0) / 100.0) << " seconds\n";
    }

    if (opt.wigFile)
    {
        double start = get_wall_time();
        if (opt.outputType == OutputType::mappability)
            saveWig<true>(c, output_path, chromNames, chromLengths);
        else
            saveWig<false>(c, output_path, chromNames, chromLengths);
        if (opt.verbose)
            std::cout << "- WIG file written in " << (round((get_wall_time() - start) * 100.0) / 100.0) << " seconds\n";
    }

    if (opt.bedFile)
    {
        double start = get_wall_time();
        if (opt.outputType == OutputType::mappability)
            saveBed<true>(c, output_path, chromNames, chromLengths);
        else
            saveBed<false>(c, output_path, chromNames, chromLengths);
        if (opt.verbose)
            std::cout << "- BED file written in " << (round((get_wall_time() - start) * 100.0) / 100.0) << " seconds\n";
    }

    if (opt.csvFile)
    {
        double start = get_wall_time();
        if (opt.outputType == OutputType::mappability)
            saveCsv<true>(output_path, locations, searchParams, directoryInformation);
        else
        
//<<<<<<< HEAD
        {
            std::cout << "Create bwd Index ... " << std::flush;
            indexCreate(bwdIndex, FibreSALF());
            std::cout << "done!\n";
        }
        clear(getFibre(getFibre(getFibre(bwdIndex, FibreSA()), FibreSparseString()), FibreValues()));
        clear(getFibre(getFibre(getFibre(bwdIndex, FibreSA()), FibreSparseString()), FibreIndicators()));
        genmap::detail::saveRev(bwdIndex, toCString(std::string(toCString(options.indexPath)) + ".rev"));
//=======

            saveCsv<false>(output_path, locations, searchParams, directoryInformation);
        if (opt.verbose)
            std::cout << "- CSV file written in " << (round((get_wall_time() - start) * 100.0) / 100.0) << " seconds\n";
//>>>>>>> 7fbde137c6c9d173a5ae4483a7895355c36c374a
    }

    if (!opt.verbose)
        std::cout << " done!\n";
}

*/

template <typename TDistance, typename value_type, bool csvComputation, typename TSeqNo, typename TSeqPos,
          typename TIndex, typename TText, typename TChromosomeNames, typename TChromosomeLengths, typename TDirectoryInformation>
inline void run(TIndex & index, TText const & text, Options const & opt, SearchParams const & searchParams,
                std::string const & fastaFile, TChromosomeNames const & chromNames, TChromosomeLengths const & chromLengths,
                TDirectoryInformation const & directoryInformation, std::vector<TSeqNo> const & mappingSeqIdFile)
{
    std::vector<value_type> c(length(text), 0);

    std::map<Pair<TSeqNo, TSeqPos>,
             std::pair<std::vector<Pair<TSeqNo, TSeqPos> >,
                       std::vector<Pair<TSeqNo, TSeqPos> > > > locations;

    double start = get_wall_time();
    switch (opt.errors)
    {
        case 0:  computeMappability<0, csvComputation>(index, text, c, searchParams, opt.directory, chromLengths, locations, mappingSeqIdFile);
                 break;
        case 1:  computeMappability<1, csvComputation>(index, text, c, searchParams, opt.directory, chromLengths, locations, mappingSeqIdFile);
                 break;
        case 2:  computeMappability<2, csvComputation>(index, text, c, searchParams, opt.directory, chromLengths, locations, mappingSeqIdFile);
                 break;
        case 3:  computeMappability<3, csvComputation>(index, text, c, searchParams, opt.directory, chromLengths, locations, mappingSeqIdFile);
                 break;
        case 4:  computeMappability<4, csvComputation>(index, text, c, searchParams, opt.directory, chromLengths, locations, mappingSeqIdFile);
                 break;
        default: std::cerr << "E > 4 not yet supported.\n";
                 exit(1);
    }
    SEQAN_IF_CONSTEXPR (outputProgress)
    {
        std::cout << '\r';
        std::cout << "Progress: 100.00%\n" << std::flush;
    }

    if (opt.verbose)
        std::cout << "Mappability computed in " << (round((get_wall_time() - start) * 100.0) / 100.0) << " seconds\n";

    outputMappability(c, opt, searchParams, fastaFile, chromNames, chromLengths, locations, directoryInformation);
}

template <typename TChar, typename TAllocConfig, typename TDistance, typename value_type, bool csvComputation,
          typename TSeqNo, typename TSeqPos, typename TBWTLen>
inline void run(Options const & opt, SearchParams const & searchParams)
{
    typedef String<TChar, TAllocConfig> TString;
    typedef StringSet<TString, Owner<ConcatDirect<SizeSpec_<TSeqNo, TSeqPos> > > > TStringSet;

    using TFMIndexConfig = TGemMapFastFMIndexConfig<TBWTLen>;
    TFMIndexConfig::SAMPLING = opt.sampling;

    using TIndex = Index<TStringSet, TBiIndexConfig<TFMIndexConfig> >;
    TIndex index;
    open(index, toCString(opt.indexPath), OPEN_RDONLY);

    StringSet<CharString, Owner<ConcatDirect<> > > directoryInformation;
    open(directoryInformation, toCString(std::string(toCString(opt.indexPath)) + ".ids"), OPEN_RDONLY);
    appendValue(directoryInformation, "dummy.entry;0;chromosomename"); // dummy entry enforces that the mappability is
                                                                       // computed for the last file in the while loop.

    std::vector<TSeqNo> mappingSeqIdFile(length(directoryInformation) - 1);
    if (searchParams.excludePseudo)
    {
        uint64_t fastaId = 0;
        std::string fastaFile = std::get<0>(retrieveDirectoryInformationLine(directoryInformation[0]));
        for (uint64_t i = 0; i < length(directoryInformation) - 1; ++i)
        {
            auto const row = retrieveDirectoryInformationLine(directoryInformation[i]);
            if (std::get<0>(row) != fastaFile)
            {
                fastaFile = std::get<0>(row);
                ++fastaId;
            }
            mappingSeqIdFile[i] = fastaId;
        }
    }

    auto const & text = indexText(index);
    StringSet<CharString, Owner<ConcatDirect<> > > chromosomeNames;
    StringSet<uint64_t> chromosomeLengths; // ConcatDirect on PODs does not seem to support clear() ...
    uint64_t startPos = 0;
    uint64_t fastaFileLength = 0;
    std::string fastaFile = std::get<0>(retrieveDirectoryInformationLine(directoryInformation[0]));
    for (uint64_t i = 0; i < length(directoryInformation); ++i)
    {
        auto const row = retrieveDirectoryInformationLine(directoryInformation[i]);
        if (std::get<0>(row) != fastaFile)
        {
            auto const & fastaInfix = infixWithLength(text.concat, startPos, fastaFileLength);
            run<TDistance, value_type, csvComputation, TSeqNo, TSeqPos>(index, fastaInfix, opt, searchParams, fastaFile, chromosomeNames, chromosomeLengths, directoryInformation, mappingSeqIdFile);

            startPos += fastaFileLength;
            fastaFile = std::get<0>(row);
            fastaFileLength = 0;
            clear(chromosomeNames);
            clear(chromosomeLengths);
        }
        fastaFileLength += std::get<1>(row);
        appendValue(chromosomeNames, std::get<2>(row));
        appendValue(chromosomeLengths, std::get<1>(row));
    }
}

template <typename TChar, typename TAllocConfig, typename TDistance, typename TValue, bool csvComputation>
inline void run(Options const & opt, SearchParams const & searchParams)
{
    if (opt.seqNoWidth == 16 && opt.maxSeqLengthWidth == 32)
    {
        if (opt.totalLengthWidth == 32)
            run<TChar, TAllocConfig, TDistance, TValue, csvComputation, uint16_t, uint32_t, uint32_t>(opt, searchParams);
        else if (opt.totalLengthWidth == 64)
            run<TChar, TAllocConfig, TDistance, TValue, csvComputation, uint16_t, uint32_t, uint64_t>(opt, searchParams);
    }
    else if (opt.seqNoWidth == 32 && opt.maxSeqLengthWidth == 16 && opt.totalLengthWidth == 64)
        run<TChar, TAllocConfig, TDistance, TValue, csvComputation, uint32_t, uint16_t, uint64_t>(opt, searchParams);
    else if (opt.seqNoWidth == 64 && opt.maxSeqLengthWidth == 64 && opt.totalLengthWidth == 64)
        run<TChar, TAllocConfig, TDistance, TValue, csvComputation, uint64_t, uint64_t, uint64_t>(opt, searchParams);
}

template <typename TChar, typename TAllocConfig, typename TDistance, typename TValue>
inline void run(Options const & opt, SearchParams const & searchParams)
{
    if (opt.csvFile || searchParams.excludePseudo) // compute csv output when --exclude-pseudo, but don't output!
        run<TChar, TAllocConfig, TDistance, TValue, true>(opt, searchParams);
    else
        run<TChar, TAllocConfig, TDistance, TValue, false>(opt, searchParams);
}

template <typename TChar, typename TAllocConfig, typename TDistance>
inline void run(Options const & opt, SearchParams const & searchParams)
{
    if (opt.outputType == OutputType::frequency_large || opt.outputType == OutputType::mappability) // TODO: document precision for mappability
        run<TChar, TAllocConfig, TDistance, uint16_t>(opt, searchParams);
    else // if (opt.outputType == OutputType::frequency_small)
        run<TChar, TAllocConfig, TDistance, uint8_t>(opt, searchParams);
}

template <typename TChar, typename TAllocConfig>
inline void run(Options const & opt, SearchParams const & searchParams)
{
    if (opt.indels)
    {
        std::cerr << "ERROR: EditDistance is not officially supported yet. Coming soon!\n";
        exit(1);
        // run<TChar, TAllocConfig, EditDistance>(opt, searchParams);
    }
    else
        run<TChar, TAllocConfig, HammingDistance>(opt, searchParams);
}

template <typename TChar>
inline void run(Options const & opt, SearchParams const & searchParams)
{
    if (opt.mmap)
        run<TChar, MMap<> >(opt, searchParams);
    else
        run<TChar, Alloc<> >(opt, searchParams);
}

int mappabilityMain(int argc, char const ** argv)
{
    // Argument parser
    ArgumentParser parser("GenMap map");
    sharedSetup(parser);
    addDescription(parser,
        "Tool for computing the mappability/frequency on nucleotide sequences. It supports multi-fasta files with DNA or RNA alphabets (A, C, G, T/U, N). Frequency is the absolute number of occurrences, mappability is the inverse, i.e., 1 / frequency-value.");

    addOption(parser, ArgParseOption("I", "index", "Path to the index", ArgParseArgument::INPUT_FILE, "IN"));
	setRequired(parser, "index");

    addOption(parser, ArgParseOption("O", "output", "Path to output directory", ArgParseArgument::OUTPUT_FILE, "OUT"));
    setRequired(parser, "output");

    addOption(parser, ArgParseOption("E", "errors", "Number of errors", ArgParseArgument::INTEGER, "INT"));

    addOption(parser, ArgParseOption("K", "length", "Length of k-mers", ArgParseArgument::INTEGER, "INT"));
    setRequired(parser, "length");

    addOption(parser, ArgParseOption("c", "reverse-complement", "Searches each k-mer on the reverse strand as well."));

    addOption(parser, ArgParseOption("ep", "exclude-pseudo", "Mappability only counts the number of fasta files that contain the k-mer, not the total number of occurrences (i.e., neglects so called- pseudo genes / sequences). This has no effect on the csv output."));

    addOption(parser, ArgParseOption("i", "indels", "Turns on indels (EditDistance). "
        "If not selected, only mismatches will be considered."));

    addOption(parser, ArgParseOption("fs", "frequency-small", "Stores frequencies using 8 bit per value (max. value 255) instead of the mappbility using a float per value (32 bit). Applies to all formats (raw, txt, wig, bed)."));
    addOption(parser, ArgParseOption("fl", "frequency-large", "Stores frequencies using 16 bit per value (max. value 65535) instead of the mappbility using a float per value (32 bit). Applies to all formats (raw, txt, wig, bed)."));

    addOption(parser, ArgParseOption("r", "raw",
        "Output raw files, i.e., the binary format of std::vector<T> with T = float, uint8_t or uint16_t (depending on whether -fs or -fl is set). For each fasta file that was indexed a separate file is created. File type is .map, .freq8 or .freq16."));

    addOption(parser, ArgParseOption("t", "txt",
        "Output human readable text files, i.e., the mappability respectively frequency values separated by spaces (depending on whether -fs or -fl is set). For each fasta file that was indexed a separate txt file is created. WARNING: This output is significantly larger that raw files."));

    addOption(parser, ArgParseOption("w", "wig",
        "Output wig files, e.g., for adding a custom feature track to genome browsers. For each fasta file that was indexed a separate wig file and chrom.size file is created."));

    addOption(parser, ArgParseOption("b", "bed",
        "Output bed files. For each fasta file that was indexed a separate bed-file is created."));

    addOption(parser, ArgParseOption("d", "csv",
        "Output a detailed csv file reporting the locations of each k-mer (WARNING: This will produce large files and makes computing the mappability significantly slower)."));

    addOption(parser, ArgParseOption("m", "memory-mapping",
        "Turns memory-mapping on, i.e. the index is not loaded into RAM but accessed directly from secondary-memory. This may increase the overall running time, but do NOT use it if the index lies on network storage."));

    addOption(parser, ArgParseOption("T", "threads", "Number of threads", ArgParseArgument::INTEGER, "INT"));
    setDefaultValue(parser, "threads", omp_get_max_threads());

    addOption(parser, ArgParseOption("v", "verbose", "Outputs some additional information."));

    addOption(parser, ArgParseOption("xo", "overlap", "Number of overlapping reads (xo + 1 Strings will be searched at once beginning with their overlap region). Default: K * (0.7^e * MIN(MAX(K,30),100) / 100)", ArgParseArgument::INTEGER, "INT"));
    hideOption(parser, "overlap");

    ArgumentParser::ParseResult res = parse(parser, argc, argv);
    if (res != ArgumentParser::PARSE_OK)
        return res == ArgumentParser::PARSE_ERROR;

    // Retrieve input parameters
    Options opt;
    SearchParams searchParams;
    getOptionValue(opt.errors, parser, "errors");
    getOptionValue(opt.indexPath, parser, "index");
    getOptionValue(opt.outputPath, parser, "output");

    // Check whether the output path exists
    {
        struct stat st;
        if (!(stat(toCString(opt.outputPath), &st) == 0 && S_ISDIR(st.st_mode)))
        {
            std::cerr << "ERROR: The output directory " << opt.outputPath << " does not exist\n"
                      << "       Please create it, or choose a different location.\n";
            return ArgumentParser::PARSE_ERROR;
        }

        if (back(opt.outputPath) != '/')
            appendValue(opt.outputPath, '/');
    }

    opt.mmap = isSet(parser, "memory-mapping");
    opt.indels = isSet(parser, "indels");
    opt.wigFile = isSet(parser, "wig");
    opt.bedFile = isSet(parser, "bed");
    opt.rawFile = isSet(parser, "raw");
    opt.txtFile = isSet(parser, "txt");
    opt.csvFile = isSet(parser, "csv");
    opt.verbose = isSet(parser, "verbose");

    if (!opt.wigFile && !opt.bedFile && !opt.rawFile && !opt.txtFile && !opt.csvFile)
    {
        std::cerr << "ERROR: Please choose at least one output format (i.e., --wig, --bed, --raw, --txt, --csv).\n";
        return ArgumentParser::PARSE_ERROR;
    }

    // store in temporary variables to avoid parsing arguments twice
    bool const isSetFS = isSet(parser, "frequency-small");
    bool const isSetFL = isSet(parser, "frequency-large");

    if (isSetFS && isSetFL)
    {
        std::cerr << "ERROR: Cannot use both --frequency-small and --frequency-large. Please choose one.\n";
        return ArgumentParser::PARSE_ERROR;
    }

    if (isSetFS)
        opt.outputType = OutputType::frequency_small;
    else if (isSetFL)
        opt.outputType = OutputType::frequency_large;
    else // default value
        opt.outputType = OutputType::mappability;

    getOptionValue(searchParams.length, parser, "length");
    getOptionValue(searchParams.threads, parser, "threads");
    searchParams.revCompl = isSet(parser, "reverse-complement");
    searchParams.excludePseudo = isSet(parser, "exclude-pseudo");

    // store in temporary variables to avoid parsing arguments twice
    bool const isSetOverlap = isSet(parser, "overlap");
    if (isSetOverlap)
        getOptionValue(searchParams.overlap, parser, "overlap");
    else if (opt.errors == 0)
        searchParams.overlap = searchParams.length * 0.7;
    else
        searchParams.overlap = searchParams.length * std::min(std::max(searchParams.length, 30u), 100u) * pow(0.7f, opt.errors) / 100.0;

    // (K - O >= E + 2 must hold since common overlap has length K - O and will be split into E + 2 parts)
    uint64_t const maxPossibleOverlap = std::min(searchParams.length - 1, searchParams.length - opt.errors - 2);
    if (searchParams.overlap > maxPossibleOverlap)
    {
        if (!isSetOverlap)
        {
/*
//<<<<<<< HEAD
            // skip empty sequences
            if (length(chromosomes2[i]) == 0)
                continue;

            std::string const id = toCString(static_cast<CharString>(ids[i]));
            std::string const len = std::to_string(length(chromosomes2[i]));
            std::string const file = extractFileName(toCString(fastaPath));
            // NOTE: maybe parsing of sequence names in fasta files is necessary
            appendValue(directoryInformation, file + ";" + len + ";" + id);  // Note: no newline!
            appendValue(chromosomes, chromosomes2[i]);
//=======
*/
            searchParams.overlap = maxPossibleOverlap;
//>>>>>>> 7fbde137c6c9d173a5ae4483a7895355c36c374a
        }
        else
        {
            std::cerr << "ERROR: overlap cannot be larger than min(K - 1, K - E + 2) = " << maxPossibleOverlap << ".\n";
            return ArgumentParser::PARSE_ERROR;
        }
    }

    // searchParams.overlap - length of common overlap
    searchParams.overlap = searchParams.length - searchParams.overlap;

    // TODO: error message if output files already exist or directory is not writeable
    // TODO: nice error messages if index is incorrect or doesnt exist
    if (back(opt.indexPath) != '/')
        opt.indexPath += '/';
    opt.indexPath += "index";

    StringSet<CharString, Owner<ConcatDirect<> > > info;
    std::string infoPath = std::string(toCString(opt.indexPath)) + ".info";
    open(info, toCString(infoPath));
    opt.alphabet = "dna" + retrieve(info, "alphabet_size");
    opt.seqNoWidth = std::stoi(retrieve(info, "sa_dimensions_i1"));
    opt.maxSeqLengthWidth = std::stoi(retrieve(info, "sa_dimensions_i2"));
    opt.totalLengthWidth = std::stoi(retrieve(info, "bwt_dimensions"));
    opt.sampling = std::stoi(retrieve(info, "sampling_rate"));
    opt.directory = retrieve(info, "fasta_directory") == "true";

    if (opt.verbose)
    {
        // TODO: dna5/rna5
        std::cout << "Index was loaded (" << opt.alphabet << " alphabet, sampling rate of " << opt.sampling << ").\n"
                     "- The BWT is represented by " << opt.totalLengthWidth << " bit values.\n"
                     "- The sampled suffix array is represented by pairs of " << opt.seqNoWidth <<
                     " and " << opt.maxSeqLengthWidth  << " bit values.\n";

        if (opt.directory)
            std::cout << "- Index was built on an entire directory.\n" << std::flush;
        else
            std::cout << "- Index was built on a single fasta file.\n" << std::flush;
    }

    // TODO: remove brackets, opt.alphabet and replace by local bool.
    if (opt.alphabet == "dna4")
    {
        run<Dna>(opt, searchParams);
    }
    else
    {
        run<Dna5>(opt, searchParams);
    }

    return 0;
}
