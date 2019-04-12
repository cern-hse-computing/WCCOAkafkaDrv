/** © Copyright 2019 CERN
 *
 * This software is distributed under the terms of the
 * GNU Lesser General Public Licence version 3 (LGPL Version 3),
 * copied verbatim in the file “LICENSE”
 *
 * In applying this licence, CERN does not waive the privileges
 * and immunities granted to it by virtue of its status as an
 * Intergovernmental Organization or submit itself to any jurisdiction.
 *
 * Author: Alexandru Savulescu (HSE-CEN-CO)
 *
 **/

#ifndef UTILS_HXX
#define UTILS_HXX


#include <string>
#include <algorithm>
#include <iterator>
#include <vector>
#include <future>
#include <chrono>
#include <iostream>

template <class T>
std::ostream& operator << (std::ostream& os, const std::vector<T>& iterable)
{
    for (auto & i : iterable)
        os << i << " ";
    return os;
}


namespace REMUS {

using std::future;
using std::future_status;
using std::cout;
using std::exception;
using std::endl;

class Utils
{
public:
    template <typename T>
    static bool future_is_ready(const future<T>& fut)
    {
        bool ret = false;
        try
        {
            future_status status;
            status = fut.wait_for(std::chrono::milliseconds(0));
            if (status == future_status::deferred) {
                cout << "deferred\n";
            } else if (status == future_status::timeout) {
                cout << "timeout\n";
            } else if (status == future_status::ready) {
                cout << "ready!\n";
                ret = true;
            }
            else
            {
                cout << " Ha!";
            }
        }
        catch(exception& e)
        {
            ret = true;
            cout << "No state" << endl;
        }
        return ret;
    }


    static auto split(const std::string& str, char&& delimiter = '$') -> std::vector<std::string>
    {
        std::vector<std::string> result;
        std::size_t previous = 0;
        std::size_t current = 0;

        while ((current = str.find(delimiter, previous))!= std::string::npos) {
            result.emplace_back(str.substr(previous, current - previous));
            previous = current + 1;
        }

        result.emplace_back(str.substr(previous));

        return result;
    }

    static bool convertToInt(const std::string& str, int& out)
    {
        bool ret = false;
        try
        {
            out = std::stoi(str.c_str());
            ret = true;
        }
        catch (std::exception& e)
        {
            std::cout << "Exception : " << e.what() << std::endl;
            ret = false;
        }

        return ret;
    }
};


class DataRAII
{

};

}

#endif // UTILS_HXX
