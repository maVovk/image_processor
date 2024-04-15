#include "../utils/processor.h"
#include "../utils/console_interface.h"

namespace {
const std::map<std::string, AbstractFilter*> FILTERS_ALIASES{{CropFilter::ALIAS, new CropFilter()},
                                                             {GrayscaleFilter::ALIAS, new GrayscaleFilter()},
                                                             {NegativeFilter::ALIAS, new NegativeFilter()},
                                                             {SharpeningFilter::ALIAS, new SharpeningFilter()},
                                                             {EdgeDetectionFilter::ALIAS, new EdgeDetectionFilter()},
                                                             {GaussianBlurFilter::ALIAS, new GaussianBlurFilter()}};

}

void ImageProcessor(int argc, char** argv) {
    if (argc == 1 || argc == 2) {
        console_interface::Help();  // вывод справки юзеру
        return;
    }

    std::string input_path = argv[1];
    std::string output_path = argv[2];

    if (!input_path.ends_with(".bmp") || !output_path.ends_with(".bmp")) {
        throw UnsupportedFileFormat{"Not .bmp"};
    }

    Image* img = nullptr;
    img = bmp_reader::ReadFile(input_path, img);

    size_t start = 3;

    while (start != static_cast<size_t>(argc)) {
        std::string filter_alias;
        std::queue<std::string> parameters;
        start = console_interface::ParseArguments(argv, start, argc, filter_alias, parameters);

        try {
            FILTERS_ALIASES.at(filter_alias)->Apply(*img, parameters);
        } catch (const std::out_of_range& e) {
            throw InvalidArgumentsError{};
        }

        filter_alias.clear();
        console_interface::Clear(parameters);
    }

    bmp_reader::SaveFile(output_path, img);

    for (auto [key, val] : FILTERS_ALIASES) {
        delete val;
    }

    delete img;
}