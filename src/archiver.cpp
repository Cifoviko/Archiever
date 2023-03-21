#include "command_line_parser.h"
#include "file_io.h"
#include "coder.h"

#include <iostream>
#include <string>
#include <cstdint>

using Symbol = int32_t;

const Symbol FILENAME_END = 256;
const Symbol ONE_MORE_FILE = 257;
const Symbol ARCHIVE_END = 258;

void PrintHelp() {  // Print usage help
    std::cout
        << "archiver -c archive_name file1 [file2 ...] - заархивировать файлы file1, file2, ... и сохранить "
           "результат в файл archive_name."
        << std::endl
        << "archiver -d archive_name - разархивировать файлы из архива archive_name и положить в текущую директорию."
        << std::endl
        << "archiver -h - вывести справку по использованию программы." << std::endl
        << "archiver -b input_file output_file - создать output_file из символов 0 и 1 соответствующий битовому "
           "представлению файла input_file"
        << std::endl;
}

void CreateVisibleBitFile(const std::string& file_input,
                          const std::string& file_output) {  // Creating file_output file which translate each bit of
                                                             // file_input file into '0' or '1' respectively
    BitFileIO input(file_input, IO_READ);
    BitFileIO output(file_output, IO_WRITE);

    while (true) {
        Symbol bit = input.ReadWord(1);  // Read one bit

        if (bit < 0) {  // Check on file end
            break;
        } else if (bit == 1) {
            output.WriteWord(static_cast<Symbol>('1'), 8);
        } else if (bit == 0) {
            output.WriteWord(static_cast<Symbol>('0'), 8);
        }
    }
}

void Archive(const std::vector<std::string>& files) {  // Archiving files[1..n] in arrchive files[0]
    const std::string& archive_name = files[0];
    BitFileIO output(archive_name, IO_WRITE);
    BitFileIO input;

    Coder<Symbol> coder;
    for (size_t id = 1; id < files.size(); ++id) {
        const std::string& file = files[id];
        coder.Reset();

        // [begin] Counting symbol frequencies
        coder.AddSymbol(FILENAME_END);   // Adding service symbols
        coder.AddSymbol(ONE_MORE_FILE);  // Adding service symbols
        coder.AddSymbol(ARCHIVE_END);    // Adding service symbols

        for (const auto& symbol : file) {
            coder.AddSymbol(static_cast<Symbol>(symbol));  // Adding file_name symbols
        }

        input.CloseFile();
        input.OpenFile(file, IO_READ);
        while (true) {
            Symbol symbol = input.ReadWord(8);
            if (symbol < 0) {
                break;
            }
            coder.AddSymbol(symbol);  // Adding file_conten symbols
        }
        // [end] Counting symbol frequencies

        coder.Encode();                  // Calculating symbols coded lengths
        coder.CreateCanonHuffmanTree();  // Calculating canonical codes

        // [begin] Getting (length-symbol) table
        size_t symbols_count = coder.GetEncodedList().size();
        std::vector<std::pair<size_t, Symbol>> encoded_table;
        for (const auto& [symbol, length] : coder.GetEncodedList()) {
            encoded_table.push_back(std::make_pair(length, symbol));
        }
        sort(encoded_table.begin(), encoded_table.end());
        // [end] Getting (length-symbol) table

        // [begin] Outputting (length-symbol) table
        output.WriteWord(static_cast<Symbol>(symbols_count), 9);
        for (const auto& [length, symbol] : encoded_table) {
            output.WriteWord(symbol, 9);
        }
        // [begin] Outputting (length-symbol) table

        // [begin] Getting lengths table
        size_t max_length = encoded_table.back().first;
        std::vector<size_t> lengths(max_length);
        for (const auto& [length, symbol] : encoded_table) {
            lengths[length - 1]++;
        }
        // [end] Getting lengths table

        // [begin] Outputting lengths table
        for (auto count : lengths) {
            output.WriteWord(static_cast<Symbol>(count), 9);
        }
        // [end] Outputting lengths table

        // [begin] Outputting coded file_name
        for (const auto& symbol : file) {
            output.WriteWord(coder.GetCode(symbol));
        }
        output.WriteWord(coder.GetCode(FILENAME_END));
        // [end] Outputting coded file_name

        // [begin] Outputting coded file_content
        input.CloseFile();
        input.OpenFile(file, IO_READ);
        while (true) {
            Symbol symbol = input.ReadWord(8);
            if (symbol < 0) {  // Check on file end
                break;
            }
            output.WriteWord(coder.GetCode(symbol));
        }
        if (id + 1 == files.size()) {  // Check is there file after
            output.WriteWord(coder.GetCode(ARCHIVE_END));
        } else {
            output.WriteWord(coder.GetCode(ONE_MORE_FILE));
        }
        // [end] Outputting coded file_content
    }
}

Symbol ReadSymbol(BitFileIO& input, Coder<Symbol>& coder) {  // Descending in coded bor to get symbol using code
    coder.ResetNode();
    while (!coder.GoToNode(input.ReadWord(1))) {
    }
    return coder.GetNodeValue();
}

void Unarchive(const std::string& archive) {
    BitFileIO input(archive, IO_READ);
    BitFileIO output;
    Coder<Symbol> coder;

    while (true) {
        // [begin] reading (length-symbol) table
        size_t symbols_count = static_cast<size_t>(input.ReadWord(9));
        std::vector<std::pair<size_t, Symbol>> encoded_table;
        for (size_t id = 0; id < symbols_count; ++id) {
            encoded_table.push_back(std::make_pair(0, static_cast<Symbol>(input.ReadWord(9))));
        }
        // [end] reading (length-symbol) table

        // [begin] reading lengths table
        size_t lengths_sum = 0;
        size_t length = 1;
        while (lengths_sum < symbols_count) {
            for (size_t cnt = static_cast<size_t>(input.ReadWord(9)); cnt > 0; --cnt) {
                encoded_table[lengths_sum].first = length;
                ++lengths_sum;
            }
            ++length;
        }
        // [end] reading lengths table

        coder.CreateCanonHuffmanTree(encoded_table);  // creating bor on codes

        // [begin] reading file_name
        std::string file_name;
        while (true) {
            Symbol symbol = ReadSymbol(input, coder);
            if (symbol == FILENAME_END) {
                break;
            }
            file_name += static_cast<char>(symbol);
        }
        // [end] reading file_name

        // [begin] reading file_content
        output.OpenFile(file_name, IO_WRITE);
        Symbol symbol = 0;
        while (true) {
            symbol = ReadSymbol(input, coder);
            if (symbol == ARCHIVE_END || symbol == ONE_MORE_FILE) {
                break;
            }
            output.WriteWord(symbol, 8);
        }
        output.CloseFile();
        // [end] reading file_content

        if (symbol == ARCHIVE_END) {  // checking is it last file
            break;
        }
    }
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "Russian");
    try {
        CommandLineParser parser(argc, argv);
        switch (parser.GetCommand()) {
            case (parser_setup::Command::HELP):
                PrintHelp();  // Command Help (-h)
                break;
            case (parser_setup::Command::ARCHIVE):
                Archive(parser.GetParameters());  // Command create Archive (-c)
                break;
            case (parser_setup::Command::UNARRCHIVE):
                Unarchive(parser.GetParameter(0));  // Command Unarchive (-d)
                break;
            case (parser_setup::Command::BIT_VIEW):
                CreateVisibleBitFile(parser.GetParameter(0),
                                     parser.GetParameter(1));  // Command create Char bit file (-b)
                break;
            case (parser_setup::Command::Unknown):
                return 111;
            default:
                return 111;
        }
    } catch (const Exeption& error) {
        std::cout << '-' << error.GetFeedback() << std::endl;
        return 111;
    } catch (const std::exception& error) {
        std::cout << "Fatal error has occured:" << std::endl;
        std::cout << error.what() << std::endl;
        return 111;
    }

    return 0;
}