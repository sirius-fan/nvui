#ifndef NVUI_HLSTATE_HPP
#define NVUI_HLSTATE_HPP

#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <variant>
#include <vector>
#include <string>
#include <msgpack.hpp>

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;

enum Kind
{
  Syntax,
  UI
};

struct Color
{
  uint8 r;
  uint8 g;
  uint8 b;
  Color() = default;
  Color(uint32 clr)
    : r((clr & 0x00ff0000) >> 16),
      g((clr & 0x0000ff00) >> 8),
      b((clr & 0x000000ff))
  {
  }
};

struct AttrState
{
  Kind kind;
  std::string hi_name;
  std::string ui_name;
  uint16 id;
};

/// Data for a single highlight attribute
class HLAttr
{
public:
  int hl_id;
  bool reverse;
  Color special;
  Color foreground;
  Color background;
  /// We don't need a detailed view of the highlight state
  // right now so we won't do anything with this.
  std::vector<AttrState> state;
  float opacity = 1;
  HLAttr();
  HLAttr(int id);
  HLAttr(const HLAttr& other);
  HLAttr(HLAttr&& other);
  HLAttr& operator=(const HLAttr&) = default;
  HLAttr& operator=(HLAttr&&);
};

/// Keeps the highlight state of Neovim
/// HlState is essentially a map of highlight names to their
/// corresponding id's, and a secondary map of id's to
/// the HLAttr they correspond to.
class HLState
{
public:
  HLState() = default;
  /**
   * Maps name to hl_id.
   * This function maps to "hl_group_set".
   */
  void set_name_id(const std::string& name, std::uint32_t hl_id);
  /**
   * Maps id to attr.
   */
  void set_id_attr(int id, HLAttr attr);
  /**
   * Returns the highlight attribute for the given id.
   */
  const HLAttr& attr_for_id(int id) const;
  /**
   * Returns the name of the highlight group for the given id.
   */
  int id_for_name(const std::string& name) const;
  /**
   * Manages an "hl_attr_define" call, with obj
   * being the parameters of the call.
   */
  void define(const msgpack::object& obj);
  /**
   * Sets the default colors.
   */
  void default_colors_set(const msgpack::object& obj);
  /**
   * Sets the given highlight group. This should be called with
   * the parameters of an "hl_group_set" call.
   */
  void group_set(const msgpack::object& obj);
  /**
   * Returns the default colors.
   */
  const HLAttr& default_colors_get() const;
private:
  HLAttr default_colors;
  std::unordered_map<std::string, std::uint32_t> name_to_id;
  std::unordered_map<int, HLAttr> id_to_attr;
};

/// Defining a function to parse "hl_attr_define" data
/// into an HLState (for startup, after that
/// we modify the initial state).
namespace hl
{
  /**
   * Produces an HLAttr from the given object.
   * obj must be of type msgpack::type::ARRAY,
   * and should only be called with arrays
   * that were the parameters of an "hl_attr_define"
   * call.
   */
  HLAttr hl_attr_from_object(const msgpack::object& obj);
}

#endif
