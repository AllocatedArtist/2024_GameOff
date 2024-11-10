#include "InputManager.h"


InputManager::InputManager() {
  
}

std::optional<std::string> InputManager::GetAction(Key key) const {
  if (action_map_.find(KeyInt(key)) == action_map_.cend()) {
    return {};
  }
  return action_map_.at(KeyInt(key));
}

void InputManager::AddAction(Key key, const std::string& action) {
  action_map_[KeyInt(key)] = action;
}

bool InputManager::ActionExists(const std::string& action) const {
  return actions_.find(action) != actions_.cend();
}

bool InputManager::IsActionUp(const std::string& action) const {
  if (!ActionExists(action)) {
    return false;
  }
  ActionType type = actions_.at(action).type_;
  return type == ActionType::kRelease;
}

bool InputManager::IsActionDown(const std::string& action) const {
  if (!ActionExists(action)) {
    return false;
  }
  ActionType type = actions_.at(action).type_;
  return type == ActionType::kPress;
}

void InputManager::RegisterInputs() {
  for (auto& [_, action] : action_map_) {
    actions_.insert(std::make_pair(action, Action(action, ActionType::kRelease)));
  }
}
