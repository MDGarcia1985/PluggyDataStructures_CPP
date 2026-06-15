/*
 * File: FileLoader.cpp
 * Description: Discovers and loads generic text and CSV datasets.
 * Copyright (c) 2026 Michael Garcia
 * Contact: michael@mandedesign.studio
 * Site: https://mandedesign.studio
 * SPDX-License-Identifier: MIT
 */

#include "io/FileLoader.h"


// named container llb = linked list browser
namespace llb
{
    /*
     * Purpose: Discover every regular file available in the configured data directory.
     * Design: Returns sorted paths so the landing menu is stable and generated from disk contents.
     * Workflow: Inspect data/, collect regular files, sort their paths, and return the collection.
     * Data Handoff: Supplies Menu with selectable dataset paths, including unsupported file types.
     */
    std::vector<std::string> FileLoader::discoverDataFiles()
    {
        std::vector<std::string> files;
        const std::filesystem::path dataDirectory(DATA_DIRECTORY);

        std::error_code error;
        if (!std::filesystem::exists(dataDirectory, error))
        {
            return files;
        }

        for (const std::filesystem::directory_entry& entry :
            std::filesystem::directory_iterator(dataDirectory, error))
        {
            if (!error && entry.is_regular_file())
            {
                files.push_back(entry.path().generic_string());
            }
        }

        std::sort(files.begin(), files.end(),
            [](const std::string& left, const std::string& right)
            {
                return std::filesystem::path(left).filename().string()
                    < std::filesystem::path(right).filename().string();
            });

        return files;
    }

    /*
     * Purpose: Classify a dataset by its filename extension.
     * Design: Normalizes the extension to lowercase before applying the supported type rules.
     * Workflow: Read the extension, lowercase it, and map txt/csv to their loader categories.
     * Data Handoff: Tells Menu and loadTargetsFromFile() which behavior applies to a selected path.
     */
    DataFileType FileLoader::fileType(const std::string& filePath)
    {
        std::string extension = std::filesystem::path(filePath).extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(),
            [](unsigned char character)
            {
                return static_cast<char>(std::tolower(character));
            });

        if (extension == ".txt")
        {
            return DataFileType::Text;
        }
        if (extension == ".csv")
        {
            return DataFileType::Csv;
        }

        return DataFileType::Unsupported;
    }

    /*
     * Purpose: Load a supported dataset into a TargetList.
     * Design: Dispatches by extension so format-specific parsing stays in focused private helpers.
     * Workflow: Identify the file type, call the matching loader, or reject unsupported extensions.
     * Data Handoff: Receives a selected path and fills the caller-owned TargetList with parsed records.
     */
    bool FileLoader::loadTargetsFromFile(const std::string& filePath, TargetList& targets)
    {
        if (fileType(filePath) == DataFileType::Text)
        {
            return loadTextTargets(filePath, targets);
        }
        if (fileType(filePath) == DataFileType::Csv)
        {
            return loadCsvTargets(filePath, targets);
        }

        return false;
    }

    /*
     * Purpose: Load a selected dataset while guaranteeing a usable Target collection.
     * Design: Centralizes fallback policy so every data structure receives the same source data.
     * Workflow: Clear existing records, try the selected file, and load built-in records on failure.
     * Data Handoff: Fills the caller-owned TargetList and reports whether the selected file loaded.
     */
    bool FileLoader::loadTargetsOrFallback(const std::string& filePath, TargetList& targets)
    {
        targets.clear();

        if (loadTargetsFromFile(filePath, targets))
        {
            return true;
        }

        loadFallbackTargets(targets);
        return false;
    }

    /*
     * Purpose: Provide starter target data when a directly requested file cannot be loaded.
     * Design: Keeps fallback values generic for non-interactive construction and compatibility tests.
     * Workflow: Append each built-in Target to the provided list.
     * Data Handoff: Receives a mutable TargetList and fills it with default Target objects.
     */
    void FileLoader::loadFallbackTargets(TargetList& targets)
    {
        targets.addBack(Target("Google", "https://www.google.com"));
        targets.addBack(Target("YouTube", "https://www.youtube.com"));
        targets.addBack(Target("Wikipedia", "https://www.wikipedia.org"));
        targets.addBack(Target("GitHub", "https://www.github.com"));
        targets.addBack(Target("Stack Overflow", "https://stackoverflow.com"));
        targets.addBack(Target("Reddit", "https://www.reddit.com"));
        targets.addBack(Target("Amazon", "https://www.amazon.com"));
        targets.addBack(Target("Apple", "https://www.apple.com"));
        targets.addBack(Target("Microsoft", "https://www.microsoft.com"));
        targets.addBack(Target("OpenAI", "https://www.openai.com"));
        targets.addBack(Target("LinkedIn", "https://www.linkedin.com"));
        targets.addBack(Target("Netflix", "https://www.netflix.com"));
        targets.addBack(Target("CNN", "https://www.cnn.com"));
        targets.addBack(Target("BBC", "https://www.bbc.com"));
        targets.addBack(Target("ESPN", "https://www.espn.com"));
        targets.addBack(Target("Khan Academy", "https://www.khanacademy.org"));
        targets.addBack(Target("Coursera", "https://www.coursera.org"));
        targets.addBack(Target("Mozilla", "https://www.mozilla.org"));
        targets.addBack(Target("W3Schools", "https://www.w3schools.com"));
        targets.addBack(Target("GeeksforGeeks", "https://www.geeksforgeeks.org"));
    }

    /*
     * Purpose: Locate the edge-list companion file for a selected node dataset.
     * Design: Uses the convention <base>.edges so graphs can reuse any node file by name.
     * Workflow: Strip the node file extension, append .edges, and return the path if it exists.
     * Data Handoff: Supplies the graph session with an edge file path, or empty when none exists.
     */
    std::string FileLoader::discoverEdgeFile(const std::string& nodeFilePath)
    {
        std::filesystem::path candidate(nodeFilePath);
        candidate.replace_extension(".edges");

        std::error_code error;
        if (std::filesystem::exists(candidate, error) && !error)
        {
            return candidate.generic_string();
        }

        return "";
    }

    /*
     * Purpose: Load weighted edges and classify the outcome for accurate graph diagnostics.
     * Design: Distinguishes missing, loaded, empty, and invalid files while counting skipped rows.
     * Workflow: Read cleaned lines, honor #directed, parse valid records, and classify the result.
     * Data Handoff: Returns parsed EdgeRecords plus status and skipped-row metadata.
     */
    EdgeLoadResult FileLoader::loadEdges(const std::string& edgeFilePath)
    {
        EdgeLoadResult result;
        std::ifstream inputFile(edgeFilePath);
        if (!inputFile.is_open())
        {
            return result;
        }

        bool directed = false;
        bool sawDataRow = false;
        std::string line;
        while (std::getline(inputFile, line))
        {
            line = trim(line);
            if (line.empty())
            {
                continue;
            }

            if (line[0] == '#')
            {
                std::string directive = line.substr(1);
                directive = trim(directive);
                std::transform(directive.begin(), directive.end(), directive.begin(),
                    [](unsigned char character)
                    {
                        return static_cast<char>(std::tolower(character));
                    });
                if (directive == "directed")
                {
                    directed = true;
                }
                continue;
            }

            std::vector<std::string> fields;
            if (line.find('|') != std::string::npos)
            {
                std::string field;
                std::istringstream stream(line);
                while (std::getline(stream, field, '|'))
                {
                    fields.push_back(trim(field));
                }
            }
            else
            {
                fields = parseCsvRow(line);
            }

            std::string lowerFirst = fields.empty() ? "" : fields[0];
            std::transform(lowerFirst.begin(), lowerFirst.end(), lowerFirst.begin(),
                [](unsigned char character)
                {
                    return static_cast<char>(std::tolower(character));
                });
            if (lowerFirst == "from")
            {
                continue;
            }

            sawDataRow = true;
            if (fields.size() < 2 || fields[0].empty() || fields[1].empty())
            {
                ++result.skippedRowCount;
                continue;
            }

            EdgeRecord edge;
            edge.from = fields[0];
            edge.to = fields[1];
            edge.directed = directed;
            if (fields.size() >= 3 && !fields[2].empty())
            {
                try
                {
                    edge.weight = std::stod(fields[2]);
                }
                catch (const std::exception&)
                {
                    edge.weight = 1.0;
                }
            }

            result.edges.push_back(edge);
        }

        if (!result.edges.empty())
        {
            result.status = EdgeLoadStatus::Loaded;
        }
        else if (sawDataRow)
        {
            result.status = EdgeLoadStatus::Invalid;
        }
        else
        {
            result.status = EdgeLoadStatus::Empty;
        }

        return result;
    }

    /*
     * Purpose: Remove leading and trailing whitespace from input text.
     * Design: Preserves interior text while normalizing spaces, tabs, and line endings at the edges.
     * Workflow: Locate the first and last non-whitespace characters and return that substring.
     * Data Handoff: Converts raw file text into cleaned fields used by both supported loaders.
     */
    std::string FileLoader::trim(const std::string& text)
    {
        const std::string whitespace = " \t\n\r";
        const std::size_t first = text.find_first_not_of(whitespace);

        if (first == std::string::npos)
        {
            return "";
        }

        const std::size_t last = text.find_last_not_of(whitespace);
        return text.substr(first, last - first + 1);
    }

    /*
     * Purpose: Parse one standard CSV row into cleaned fields.
     * Design: Supports quoted commas and doubled quote escapes without dataset-specific assumptions.
     * Workflow: Walk characters, track quote state, split unquoted commas, and trim completed fields.
     * Data Handoff: Converts one raw CSV line into a vector used by the generic table loader.
     */
    std::vector<std::string> FileLoader::parseCsvRow(const std::string& line)
    {
        std::vector<std::string> fields;
        std::string field;
        bool insideQuotes = false;

        for (std::size_t index = 0; index < line.size(); ++index)
        {
            const char current = line[index];

            if (current == '"')
            {
                if (insideQuotes && index + 1 < line.size() && line[index + 1] == '"')
                {
                    field.push_back('"');
                    ++index;
                }
                else
                {
                    insideQuotes = !insideQuotes;
                }
            }
            else if (current == ',' && !insideQuotes)
            {
                fields.push_back(trim(field));
                field.clear();
            }
            else
            {
                field.push_back(current);
            }
        }

        fields.push_back(trim(field));
        return fields;
    }

    /*
     * Purpose: Load plain text or pipe-delimited list records.
     * Design: Accepts existing two-field rows and ordinary one-item lines while skipping comments.
     * Workflow: Read cleaned lines, split the first pipe when present, and append each valid item.
     * Data Handoff: Maps TXT content into generic Target values stored in the supplied list.
     */
    bool FileLoader::loadTextTargets(const std::string& filePath, TargetList& targets)
    {
        std::ifstream inputFile(filePath);
        if (!inputFile.is_open())
        {
            return false;
        }

        std::string line;
        std::size_t loadedCount = 0;

        while (std::getline(inputFile, line))
        {
            line = trim(line);
            if (line.empty() || line[0] == '#')
            {
                continue;
            }

            const std::size_t separatorIndex = line.find('|');
            if (separatorIndex == std::string::npos)
            {
                targets.addBack(Target(line, ""));
                ++loadedCount;
                continue;
            }

            const std::string fieldOne = trim(line.substr(0, separatorIndex));
            const std::string fieldTwo = trim(line.substr(separatorIndex + 1));
            if (!fieldOne.empty())
            {
                targets.addBack(Target(fieldOne, fieldTwo));
                ++loadedCount;
            }
        }

        return loadedCount > 0;
    }

    /*
     * Purpose: Load structured CSV rows into the application's two-field Target representation.
     * Design: Uses the first row as headers, the first column as field one, and labeled remaining values as field two.
     * Workflow: Parse headers, validate row width, compose labeled details, and append each data row.
     * Data Handoff: Maps a generic CSV table into Target records without knowing the table's domain.
     */
    bool FileLoader::loadCsvTargets(const std::string& filePath, TargetList& targets)
    {
        std::ifstream inputFile(filePath);
        if (!inputFile.is_open())
        {
            return false;
        }

        std::string line;
        std::vector<std::string> headers;
        while (headers.empty() && std::getline(inputFile, line))
        {
            if (!trim(line).empty())
            {
                headers = parseCsvRow(line);
            }
        }

        if (headers.empty())
        {
            return false;
        }

        std::size_t loadedCount = 0;
        while (std::getline(inputFile, line))
        {
            if (trim(line).empty())
            {
                continue;
            }

            const std::vector<std::string> row = parseCsvRow(line);
            if (row.empty() || row[0].empty())
            {
                continue;
            }

            std::string details;
            for (std::size_t index = 1; index < row.size(); ++index)
            {
                if (row[index].empty())
                {
                    continue;
                }

                if (!details.empty())
                {
                    details += "; ";
                }

                const std::string label = index < headers.size() && !headers[index].empty()
                    ? headers[index]
                    : "column " + std::to_string(index + 1);
                details += label + "=" + row[index];
            }

            targets.addBack(Target(row[0], details));
            ++loadedCount;
        }

        return loadedCount > 0;
    }
}
