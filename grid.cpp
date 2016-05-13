#include "grid.h"
#include "math.h"

/* the possible directions for an action */
// N(-1, 0) S(1, 0) W(0, -1) E(0, 1)
const int direction_set[4][3][2] = {
  { 
    {-1, 0}, 
    {0, -1}, 
    {0, 1}
  },
  { 
    {1, 0}, 
    {0, -1}, 
    {0, 1}
  },
  { 
    {0, -1}, 
    {-1, 0}, 
    {1, 0}
  },
  { 
    {0, 1}, 
    {-1, 0},
    {1, 0} 
  }
};

/* probability for 3 possible directions */
const double proba_set[3] = {DIR1_PROBA, DIR2_PROBA, DIR2_PROBA};

/* direction names */
const char *dir_names_short[] = {"N", "S", "W", "E"};


void Grid::Intialize(void) {
  /*
   * add
   */
  for (int i = 0; i < nrow_; i++) {
    for (int j = 0; j < ncol_; j++) {
      states_[i][j].set_value(0);
      states_[i][j].set_policy(0);
    }
  }
}

void Grid::PolicyIteration(void) {
  cout << endl << "Policy Iteration" << endl << endl;
  /* 
   * add
   */
  int cnt = 0;
  Intialize();

  cout << "iter: " << cnt << endl;
  print_values();
  print_policy();
  cout << endl;
  
  PolicyEvaluation();
  while(!PolicyImprovement()) {
    cnt++;
    cout << "iter: " << cnt << endl;
    print_values();
    print_policy();
    cout << endl;
    PolicyEvaluation();
  }
  
  cnt++;
  cout << "iter: " << cnt << endl;
  print_values();
  print_policy();
  cout << endl;
}


void Grid::ValueIteration(void) {
  cout << endl << "Value Iteration" << endl << endl;
  int cnt = 0;
  
  Intialize();
  double delta = THETA_DEFALT;
  double v = 0;
  double tempvalue = 0;
  double tempmax = 0;
  double tempdir = 0;
  Action act;

  cout << "iter: " << cnt << endl;
  print_values();
  cout << endl;
  while (delta >= THETA_DEFALT) {
    delta = 0;
    for (int i = 0; i < nrow_; i++) {
      for (int j = 0; j < ncol_; j++) {
        // store the current value
        v = states_[i][j].value();
        
        // first initialize the max value to be the one with given policy 
        act = generate_action(states_[i][j].policy(), StateID(i, j));
        tempmax = 0;
        tempdir = states_[i][j].policy();
        for (int k = 0; k < act.size(); k++) {
          tempmax += act[k].proba * (act[k].reward + GAMMA * states_[act[k].id.row][act[k].id.col].value());
        }
        
        // calculate the value of each action
        for (int l = 0; l < 4; l++) {
          // getting the results of the action
          act = generate_action(l, StateID(i, j));
          tempvalue = 0;
        
          for (int m = 0; m < act.size(); m++) {
            tempvalue += act[m].proba * (act[m].reward + GAMMA * states_[act[m].id.row][act[m].id.col].value());
          }
          if ((tempvalue > tempmax) || ((tempvalue == tempmax) && (l < tempdir))) {
            tempmax = tempvalue;
            tempdir = l;
          }
        }
        states_[i][j].set_value(tempmax);
        states_[i][j].set_policy(tempdir);
        
        // update the delta of the iteration
        if (abs(states_[i][j].value() - v) > delta) {
          delta = abs(states_[i][j].value() - v);
        }
      }
    } 
    cnt++;
    cout << "iter: " << cnt << endl;
    cout << "delta: " << delta << endl;
    print_values();
    cout << endl;
  }

  /* determine policy */
  
  cout << "final policy: " << endl;
  print_policy();
  cout << endl;
}


void Grid::PolicyEvaluation(void) {
  double delta = THETA_DEFALT;
  double originalvalue = 0;
  double tempvalue = 0;
  Action act;
  
  while (delta >= THETA_DEFALT) {
//    print_values();
//    print_policy();
//    cout << "delta: " << delta << endl;
    delta = 0;
    for (int i = 0; i < nrow_; i++) {
      for (int j = 0; j < ncol_; j++) {
        originalvalue = states_[i][j].value();
        act = generate_action(states_[i][j].policy(), StateID(i, j));
        tempvalue = 0;
        
        // calculating the value by using the given policy
        for (int l = 0; l < act.size(); l++) {
          tempvalue += act[l].proba * (act[l].reward + GAMMA * states_[act[l].id.row][act[l].id.col].value());
        }
        states_[i][j].set_value(tempvalue);
        
        if (abs(originalvalue - states_[i][j].value()) > delta) {
          delta = abs(originalvalue - states_[i][j].value());
        }
      }
    }
  }
}

bool Grid::PolicyImprovement(void) {
  bool stable = true;
  unsigned b;
  double tempvalue = 0;
  double tempmax = 0;
  double tempdir = 0;
  Action act;
  
  for (int i = 0; i < nrow_; i++) {
    for (int j = 0; j < ncol_; j++) {
      // store the current policy value
      b = states_[i][j].policy();
      
      // first initialize the max value equals to the policy value
      act = generate_action(states_[i][j].policy(), StateID(i, j));
      tempmax = 0;
      tempdir = states_[i][j].policy();
      for (int k = 0; k < act.size(); k++) {
        tempmax += act[k].proba * (act[k].reward + GAMMA * states_[act[k].id.row][act[k].id.col].value());
      }
      
      // calculate the value of each action
      for (int l = 0; l < 4; l++) {
        // getting the results of the action
        act = generate_action(l, StateID(i, j));
        tempvalue = 0;

        for (int m = 0; m < act.size(); m++) {
          tempvalue += act[m].proba * (act[m].reward + GAMMA * states_[act[m].id.row][act[m].id.col].value());
        }
        // replace max if it is larger or (it is equal & with smaller direction value)
        if ((tempvalue > tempmax) || ((tempvalue == tempmax) && (l < tempdir))) {
          tempmax = tempvalue;
          tempdir = l;
          states_[i][j].set_policy(l);
        }
      }
      // change the stable variable if the policy value has ever been modified
      if (b != states_[i][j].policy()) {
        stable = false;
      }
    }
  }
  return stable;
}


Grid::Grid(unsigned row, unsigned col):
    nrow_(row), 
    ncol_(col), 
    a_(A_POS),
    ap_(AP_POS),
    b_(B_POS),
    bp_(BP_POS),
    a_reward_(AREWARD),
    b_reward_(BREWARD),
    theta_(THETA_DEFALT),
    gamma_(GAMMA)
{

  // initialize states
  for (int i = 0; i < nrow_; i++) {
    StateRow state_row;
    states_.push_back(state_row);
    //states_row_t states_row;
    for (int j = 0; j < ncol_; j++) {
      StateID current_id(i, j);
      vector<Action> actions;
      // for each state, genreate result for 4 different actions (NSWE)
      for (int k = 0; k < 4; k++) 
        actions.push_back(generate_action(k, current_id));
      
      State current_state(current_id, actions);
      states_.back().push_back(current_state);
    }
  }
}


/* generate result for an action (different next states/rewards/probabilities)*/
Action Grid::generate_action(unsigned dir, StateID current_id) const {
  Action action;

  double reward;
  double proba;

  if (current_id == a_) 
    action.push_back(NextState(ap_, 1, a_reward_));
  else if (current_id == b_) 
    action.push_back(NextState(bp_, 1, b_reward_));
  else {
    // a non-deterministic action may generate different results
    for (int i = 0; i < 3; i++) {
      int newrow =  current_id.row + direction_set[dir][i][0];
      int newcol =  current_id.col + direction_set[dir][i][1];
      
      if(isingrid(newrow, newcol)) 
        reward = 0;
      else {
        newrow = current_id.row;
        newcol = current_id.col;
        reward = -1;		
      }
      proba = proba_set[i];
      action.push_back(NextState(StateID(newrow, newcol), proba, reward));
    }
  }
  return action;
}


bool Grid::isingrid(int row, int col) const {
  return  (row >= 0) && (row < nrow_) && (col >= 0) && (col < ncol_);
}


void Grid::print(void) const {
  cout << "Grid size: (" << nrow_ << ", " << ncol_ << ")" << endl;
  cout << "Point A: " << a_.print_str() << " -> AP: " <<  ap_.print_str();
  cout << " reward: " << a_reward_ << endl;
  cout << "Point B: " << b_.print_str() << " -> BP: " <<  bp_.print_str();
  cout << " reward: " << b_reward_ << endl;
  cout << "theta: " << theta_<< endl;
  print_values();
  print_policy();
  cout << endl;
}


void Grid::print_values(void) const {
  cout << "values";
  cout << endl;
  for (int i = 0; i < nrow_; i++) {
    for (int j = 0; j < ncol_; j++) {
      cout << states_[i][j].value() << " ";
    }
    cout << endl;
  }
}


void Grid::print_policy(void) const {
  cout << "actions";
  cout << endl;
  for (int i = 0; i < nrow_; i++) {
    for (int j = 0; j < ncol_; j++) {
      cout << dir_names_short[states_[i][j].policy()] << " ";
    }
    cout << endl;
  }
}



