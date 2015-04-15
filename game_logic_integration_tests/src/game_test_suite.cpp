#include "stdafx.hpp"

#include "reversi/cell_position.hpp"
#include "reversi/file_game_logger.hpp"
#include "reversi/player.hpp"
#include <boost/filesystem.hpp>

namespace reversi { namespace testing
{

using ::testing::Eq;
using ::testing::Test;

class FileGameLogger : public Test
{

protected:

    virtual void TearDown() override
    {
        logger.close();

        boost::filesystem::remove("log.txt");
    }
    
    std::vector<std::string> get_log_lines() const
    {
        auto is = std::ifstream{"log.txt"};
        if (!is) { return {}; }

        auto line = std::string{};
        auto lines = std::vector<std::string>{};
        while (std::getline(is, line))
        {
            lines.push_back(line);
        }

        return lines;
    }

protected:

    file_game_logger logger{"log.txt"};

};

TEST_THAT(FileGameLogger,
     WHAT(LogSuccessfulPlacement),
     WHEN(Always),
     THEN(AppendsAnEntryToTheLogFile))
{
    auto lines = get_log_lines();

    auto num_of_lines = lines.size();

    logger.log_successful_placement({0, 0}, player::white);

    lines = get_log_lines();
    
    EXPECT_THAT(lines.size(), Eq(num_of_lines + 1));
}

} }