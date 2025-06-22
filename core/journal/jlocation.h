#pragma once

#include <memory>
#include <string>

#include "infra/infra.h"
#include "types.h"

namespace btra::journal {

FORWARD_DECLARE_SPTR(JLocation)
FORWARD_DECLARE_SPTR(JLocator)

/**
 * @brief 1. Hold a prefix-path of output data in the journal system.
 * 2. Compute the output path.
 */
class JLocator {
public:
    explicit JLocator(const std::string &root, enums::RunMode m, const std::vector<std::string> &tag = {});

    virtual ~JLocator() = default;

    /**
     * @brief Compute the directory where to save data according to some factors:
     * 1. root_: Root path
     * 2. module: md, td, cp or etc
     * 3. group: self-defined group
     * 4. name: self-defined name
     * 5. layout: database type
     * 6. runmode:
     * @param location
     * @param layout
     * @return std::string
     */
    [[nodiscard]] virtual std::string layout_dir(const JLocationSPtr &location, enums::layout layout) const;

    /**
     * @brief Compute the file path
     *
     * @param location
     * @param layout
     * @param name
     * @return std::string
     */
    [[nodiscard]] virtual std::string layout_file(const JLocationSPtr &location, enums::layout layout,
                                                  const std::string &name) const;

    /**
     * @brief List all existing page id of a journal.
     *
     * @param location
     * @param dest_id
     * @return std::vector<uint32_t>
     */
    [[nodiscard]] virtual std::vector<uint32_t> list_page_id(const JLocationSPtr &location, uint32_t dest_id) const;

    /**
     * @brief List all existing dest id that belong to a location
     *
     * @param location
     * @return std::vector<uint32_t>
     */
    [[nodiscard]] virtual std::vector<uint32_t> list_location_dest(const JLocationSPtr &location) const;

    bool operator==(const JLocator &another) const;

private:
    std::filesystem::path root_;
    enums::RunMode dir_mode_;
};

/**
 * @brief Determine the output directory of a journal. It's part of journal unique id.
 *
 */
struct JLocation : public std::enable_shared_from_this<JLocation>, public Location {
    static constexpr uint32_t PUBLIC = 0;
    static constexpr uint32_t SYNC = 1;

    const JLocatorSPtr locator;
    const std::string uname; /* Specified by module, runmode, */
    const uint32_t uid;      /* The result of uname hash. */

    JLocation(enums::RunMode m, enums::Module c, std::string g, std::string n, JLocatorSPtr l)
        : locator(std::move(l)),
          uname(fmt::format("{}/{}/{}/{}", enums::get_module_name(c), g, n, enums::get_mode_name(m))),
          uid(infra::hash_str_32(uname)) {
        category = c;
        group = std::move(g);
        name = std::move(n);
        mode = m;
        location_uid = uid;
    }

    template <typename T>
    inline T to() const {
        T t{};
        t.location_uid = uid;
        t.mode = mode;
        t.category = category;
        t.group = group;
        t.name = name;
        return t;
    }

    template <typename T>
    inline T &to(T &t) const {
        t.location_uid = uid;
        t.mode = mode;
        t.category = category;
        t.group = group;
        t.name = name;
        return t;
    }

    template <typename T>
    static inline JLocationSPtr make_shared(T msg, JLocatorSPtr l) {
        return std::make_shared<JLocation>(msg.mode, msg.category, msg.group, msg.name, l);
    }

    static inline JLocationSPtr make_shared(enums::RunMode m, enums::Module c, std::string g, std::string n,
                                            JLocatorSPtr l) {
        return std::make_shared<JLocation>(m, c, g, n, l);
    }
};

} // namespace btra::journal