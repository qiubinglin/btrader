#include "jlocation.h"

#include <filesystem>
#include <regex>
#include <unordered_set>

#include "jid.h"

namespace btra::journal {

std::filesystem::path get_root_dir(const std::filesystem::path &root, enums::RunMode m,
                                   const std::vector<std::string> &tags) {
  static const std::unordered_map<enums::RunMode, std::string> map_env = {
      {enums::RunMode::LIVE, "runtime"},
      {enums::RunMode::BACKTEST, "backtest"},
      {enums::RunMode::DATA, "dataset"},
      {enums::RunMode::REPLAY, "replay"},
  };

  auto iter = map_env.find(m);
  if (iter == map_env.end()) {
    throw std::runtime_error(fmt::format("unrecognized mode: {}, init root_ as mode::LIVE", static_cast<int8_t>(m)));
  }
  auto home_dir_path = root / iter->second;
  home_dir_path /= std::accumulate(tags.begin(), tags.end(), std::filesystem::path{},
                                   [&](const std::filesystem::path &p, const std::string &tag) { return p / tag; });
  return home_dir_path;
}

JLocator::JLocator(const std::string &root, enums::RunMode m, const std::vector<std::string> &tags) : dir_mode_(m) {
  root_ = get_root_dir(root, dir_mode_, tags);
}

std::string JLocator::layout_dir(const JLocationSPtr &location, enums::layout layout) const {
  auto dir = root_ /                                         //
             enums::get_module_name(location->category) /  //
             location->group /                               //
             location->name /                                //
             enums::get_layout_name(layout) /                //
             enums::get_mode_name(location->mode);
  if (not std::filesystem::exists(dir)) {
    std::filesystem::create_directories(dir);
  }
  // INFRA_LOG_DEBUG("{}", dir.string());
  return dir.string();
}

std::string JLocator::layout_file(const JLocationSPtr &location, enums::layout layout, const std::string &name) const {
  auto path =
      std::filesystem::path(layout_dir(location, layout)) / fmt::format("{}.{}", name, enums::get_layout_name(layout));
  return path.string();
}

std::vector<uint32_t> JLocator::list_page_id(const JLocationSPtr &location, uint32_t dest_id) const {
  std::vector<uint32_t> result = {};
  auto dest_id_str = fmt::format("{:08x}", dest_id);
  auto dir = std::filesystem::path(layout_dir(location, enums::layout::JOURNAL));
  for (auto &it : std::filesystem::recursive_directory_iterator(dir)) {
    auto basename = it.path().stem();
    if (it.is_regular_file() and it.path().extension() == ".journal" and basename.stem() == dest_id_str) {
      auto index = std::atoi(basename.extension().string().c_str() + 1);
      result.push_back(index);
    }
  }
  std::sort(result.begin(), result.end());
  return result;
}

static constexpr auto w = [](const std::string &pattern) { return pattern == "*" ? ".*" : pattern; };

static constexpr auto g = [](const std::string &pattern) { return fmt::format("({})", w(pattern)); };

std::vector<uint32_t> JLocator::list_location_dest(const JLocationSPtr &location) const {
  std::unordered_set<uint32_t> set = {};
  auto dir = std::filesystem::path(layout_dir(location, enums::layout::JOURNAL));
  for (auto &it : std::filesystem::recursive_directory_iterator(dir)) {
    auto basename = it.path().stem();
    if (it.is_regular_file() and it.path().extension() == ".journal") {
      set.emplace(std::stoul(basename.stem(), nullptr, 16));
    }
  }
  return std::vector<uint32_t>{set.begin(), set.end()};
}

bool JLocator::operator==(const JLocator &another) const {
  return dir_mode_ == another.dir_mode_ and root_.string() == another.root_.string();
}

}  // namespace btra::journal