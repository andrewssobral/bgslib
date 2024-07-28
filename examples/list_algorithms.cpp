#include <iterator>

#include "bgslib.hpp"

int main() {
    std::cout << "Using OpenCV " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << "." << CV_SUBMINOR_VERSION << std::endl;

    auto algorithmsName = bgslib::BGS_Factory::Instance()->GetRegisteredAlgorithmsName();

    std::cout << "List of available algorithms:" << std::endl;
    std::copy(algorithmsName.begin(), algorithmsName.end(), std::ostream_iterator<std::string>(std::cout, "\n"));

    return 0;
}
