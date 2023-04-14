#pragma once

#include "people.h"


class SatisfactionSupervisor : public PersonObserver {
public:
    SatisfactionSupervisor(int min_satisfaction, int max_satisfaction) : min_satisfaction_(min_satisfaction), max_satisfaction_(max_satisfaction) {}
    void OnSatisfactionChanged  (Person& person, int old_value, int new_value) override {
        PersonObserver::OnSatisfactionChanged(person, old_value, new_value);
        if (new_value < min_satisfaction_ && !(  new_value > old_value  )) {
            while (new_value < max_satisfaction_) {
                person.Dance();
                new_value = person.GetSatisfaction();
            }
        }
    }

private:
    int min_satisfaction_;
    int max_satisfaction_;
};

