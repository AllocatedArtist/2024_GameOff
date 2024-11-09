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

bool InputManager::IsActionPressed(const std::string& action) const {
  return actions_.at(action).type_ == ActionType::kPress;
}

bool InputManager::IsActionReleased(const std::string& action) const {
  return actions_.at(action).type_ == ActionType::kRelease;
}
