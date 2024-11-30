#include "BehaviorTree.h"

#include <stdlib.h>  // TODO: remove me

// This set of nodes provides a wide variety of behaviors and
// control flow for a behavior tree.

// 1. DecoratorNode

// This node is a basic node that wraps a single child,
// primarily used for adding additional behavior to the child.

BTStatus DecoratorTick(DecoratorNode* node) {
  return node->child->tick(node->child);  // Pass the tick to the child
}

// TODO: Constructor arguments: name, child
void CreateDecoratorNode(BTNode* child, DecoratorNode* dst) {
  dst->child = child;
  dst->base.tick = (BTStatus(*)(BTNode*))DecoratorTick;
}

// 2. NotDecorator

// This node inverts the result of its child (i.e., success becomes failure, and vice versa).

// TODO: Constructor arguments: name, child
BTStatus NotDecoratorTick(NotDecorator* node) {
  BTStatus status = node->child->tick(node->child);
  if (status == BT_SUCCESS) return BT_FAILURE;
  if (status == BT_FAILURE) return BT_SUCCESS;
  return status;
}

NotDecorator* CreateNotDecorator(BTNode* child) {
  NotDecorator* node = malloc(sizeof(NotDecorator));
  node->child = child;
  node->base.tick = (BTStatus(*)(BTNode*))NotDecoratorTick;
  return node;
}

// 3. FailIfRunningDecorator

// This node behaves like DecoratorNode but fails if its child is running.

BTStatus FailIfRunningTick(FailIfRunningDecorator* node) {
  BTStatus status = node->child->tick(node->child);
  if (status == BT_RUNNING) return BT_FAILURE;
  return status;
}

// TODO: Constructor arguments: name, child
FailIfRunningDecorator* CreateFailIfRunningDecorator(BTNode* child) {
  FailIfRunningDecorator* node = malloc(sizeof(FailIfRunningDecorator));
  node->child = child;
  node->base.tick = (BTStatus(*)(BTNode*))FailIfRunningTick;
  return node;
}

// 4. ConditionNode

// This node checks a condition function,
// succeeding if the condition is true, and failing otherwise.

BTStatus ConditionTick(ConditionNode* node) {
  return node->condition() ? BT_SUCCESS : BT_FAILURE;
}

// TODO: Constructor arguments: fn, name
ConditionNode* CreateConditionNode(bool (*condition)()) {
  ConditionNode* node = malloc(sizeof(ConditionNode));
  node->condition = condition;
  node->base.tick = (BTStatus(*)(BTNode*))ConditionTick;
  return node;
}

// 5. ConditionWaitNode

// This node runs until the condition function returns true.
// It's similar to ConditionNode, but it returns BT_RUNNING
// instead of BT_FAILURE when the condition is false.

BTStatus ConditionWaitTick(ConditionWaitNode* node) {
  return node->condition() ? BT_SUCCESS : BT_RUNNING;
}

// TODO: Const. arg.: fn, name
ConditionWaitNode* CreateConditionWaitNode(bool (*condition)()) {
  ConditionWaitNode* node = malloc(sizeof(ConditionWaitNode));
  node->condition = condition;
  node->base.tick = (BTStatus(*)(BTNode*))ConditionWaitTick;
  return node;
}

// 6. ActionNode

// This node executes an action and always succeeds after running it.

BTStatus ActionTick(ActionNode* node) {
  node->action();  // Run the action
  return BT_SUCCESS;  // Always succeeds
}

// TODO: Const. arg.: action, name
ActionNode* CreateActionNode(void (*action)()) {
  ActionNode* node = malloc(sizeof(ActionNode));
  node->action = action;
  node->base.tick = (BTStatus(*)(BTNode*))ActionTick;
  return node;
}

// 7. WaitNode

// This node waits for a specified amount of time before succeeding.

BTStatus WaitTick(WaitNode* node) {
  if (g_engine->now - node->startTime >= node->waitTime) {
    return BT_SUCCESS;
  }
  return BT_RUNNING;  // Still waiting
}

WaitNode* CreateWaitNode(u32 ms) {
  WaitNode* node = malloc(sizeof(WaitNode));
  node->waitTime = ms;
  node->startTime = 0;
  node->base.tick = (BTStatus(*)(BTNode*))WaitTick;
  return node;
}

// 8. SequenceNode

// This node goes through its children in order, succeeding if all succeed.

BTStatus SequenceTick(SequenceNode* node) {
  while (node->currentChild < node->childCount) {
    BTStatus status = node->children[node->currentChild]->tick(node->children[node->currentChild]);
    if (status == BT_RUNNING) return BT_RUNNING;
    if (status == BT_FAILURE) return BT_FAILURE;
    node->currentChild++;
  }
  return BT_SUCCESS;
}

// TODO: Const. arg.: children
SequenceNode* CreateSequenceNode(BTNode** children, u32 count) {
  SequenceNode* node = malloc(sizeof(SequenceNode));
  node->children = children;
  node->childCount = count;
  node->currentChild = 0;
  node->base.tick = (BTStatus(*)(BTNode*))SequenceTick;
  return node;
}

// 9. SelectorNode

// This node is similar to SequenceNode but succeeds
// as soon as one child succeeds.

BTStatus SelectorTick(SelectorNode* node) {
  while (node->currentChild < node->childCount) {
    BTStatus status = node->children[node->currentChild]->tick(node->children[node->currentChild]);
    if (status == BT_RUNNING || status == BT_SUCCESS) return status;
    node->currentChild++;
  }
  return BT_FAILURE;
}

BTNode* CreateSelectorNode(u32 count, BTNode** children, SelectorNode* dst) {
  dst->currentChild = 0;
  dst->childCount = count;
  dst->children = children;
  dst->base.tick = (BTStatus(*)(BTNode*))SelectorTick;
  return (BTNode*)dst;
}

// 10. LoopNode

// This node runs its children multiple times (up to maxreps).

BTStatus LoopTick(LoopNode* node) {
  while (node->currentRep < node->maxReps) {
    BTStatus status = node->children[node->currentChild]->tick(node->children[node->currentChild]);
    if (status == BT_RUNNING) return BT_RUNNING;
    if (status == BT_FAILURE) return BT_FAILURE;
    node->currentChild++;
    if (node->currentChild >= node->childCount) {
      node->currentChild = 0;  // Reset child
      node->currentRep++;
    }
  }
  return BT_SUCCESS;
}

// TODO: Const. arg.: children, maxreps
LoopNode* CreateLoopNode(BTNode** children, u32 count, u32 maxReps) {
  LoopNode* node = malloc(sizeof(LoopNode));
  node->children = children;
  node->childCount = count;
  node->maxReps = maxReps;
  node->currentRep = 0;
  node->currentChild = 0;
  node->base.tick = (BTStatus(*)(BTNode*))LoopTick;
  return node;
}

// 11. RandomNode
// The RandomNode picks a random child to execute.
// If it fails, it will try the next child
// until all children have been executed.

BTStatus RandomTick(RandomNode* node) {
  // If all children have been tried, reset currentChild
  if (node->currentChild >= node->childCount) {
    node->currentChild = 0;  // Reset for the next tick
    return BT_FAILURE;  // Failed after trying all children
  }

  // Pick a random child
  u32 randomIndex = Math__randomf(0, node->childCount, &g_engine->seeds.bt);
  BTStatus status = node->children[randomIndex]->tick(node->children[randomIndex]);

  // If the child succeeds or is still running, return that status
  if (status == BT_SUCCESS || status == BT_RUNNING) {
    return status;
  }

  // If the selected child fails, try the next one
  node->currentChild++;
  return RandomTick(node);  // Try again
}

// TODO: Const. arg.: children
RandomNode* CreateRandomNode(BTNode** children, u32 count) {
  RandomNode* node = malloc(sizeof(RandomNode));
  node->children = children;
  node->childCount = count;
  node->currentChild = 0;
  node->base.tick = (BTStatus(*)(BTNode*))RandomTick;
  return node;
}

// 12. ParallelNode
// The ParallelNode runs all its children simultaneously and
// succeeds if all children complete successfully.
// It fails if any child fails.

BTStatus ParallelTick(ParallelNode* node) {
  bool anyRunning = false;

  for (u32 i = 0; i < node->childCount; i++) {
    BTStatus status = node->children[i]->tick(node->children[i]);

    if (status == BT_RUNNING) {
      anyRunning = true;  // At least one child is still running
    } else if (status == BT_FAILURE) {
      return BT_FAILURE;  // If any child fails, the parallel fails
    }
  }

  return anyRunning ? BT_RUNNING : BT_SUCCESS;  // Succeed if all are done
}

// TODO: Const. arg.: children, name
ParallelNode* CreateParallelNode(BTNode** children, u32 count) {
  ParallelNode* node = malloc(sizeof(ParallelNode));
  node->children = children;
  node->childCount = count;
  node->base.tick = (BTStatus(*)(BTNode*))ParallelTick;
  return node;
}

// 13. ParallelNodeAny

// The ParallelNodeAny is similar to ParallelNode,
// but it succeeds if any child stops running without failing.

BTStatus ParallelAnyTick(ParallelNodeAny* node) {
  bool anyRunning = false;

  for (u32 i = 0; i < node->childCount; i++) {
    BTStatus status = node->children[i]->tick(node->children[i]);

    if (status == BT_RUNNING) {
      anyRunning = true;  // At least one child is still running
    } else if (status == BT_SUCCESS) {
      return BT_SUCCESS;  // If any child succeeds, the parallel succeeds
    }
  }

  return anyRunning ? BT_RUNNING : BT_FAILURE;  // Fail if none succeed or are running
}

// TODO: Const. arg.: children, name
ParallelNodeAny* CreateParallelNodeAny(BTNode** children, u32 count) {
  ParallelNodeAny* node = malloc(sizeof(ParallelNodeAny));
  node->children = children;
  node->childCount = count;
  node->base.tick = (BTStatus(*)(BTNode*))ParallelAnyTick;
  return node;
}

// 14. IfNode

// The IfNode checks a condition before executing its child node.
// If the condition is true, it runs the child.

BTStatus IfTick(IfNode* node) {
  if (node->condition()) {
    return node->child->tick(node->child);  // Tick the child if condition is true
  }
  return BT_FAILURE;  // Fail if the condition is false
}

// TODO: Const. arg.: cond, name, node
IfNode* CreateIfNode(bool (*condition)(), BTNode* child) {
  IfNode* node = malloc(sizeof(IfNode));
  node->condition = condition;
  node->child = child;
  node->base.tick = (BTStatus(*)(BTNode*))IfTick;
  return node;
}

// 15. PriorityNode

// The PriorityNode runs its children in order
// but only executes when a specified period has passed.
// If a child succeeds or is still running, it retains that state.

// PriorityNode allows you to execute nodes based on timing,
// creating a cooldown effect before the next execution.

// TODO: add Sleep() method which can accumulate (+=) parent priorityNode period

BTStatus PriorityTick(PriorityNode* node) {
  // Check if the period has passed since the last execution
  if (g_engine->now - node->lastExecutionTime < node->period) {
    return BT_RUNNING;  // Not enough time has passed, stay in running
  }

  node->lastExecutionTime = g_engine->now;  // Update last execution time

  // Loop through children
  for (u32 i = 0; i < node->childCount; i++) {
    BTStatus status = node->children[i]->tick(node->children[i]);
    if (status == BT_SUCCESS || status == BT_RUNNING) {
      return status;  // Return success or running
    }
  }

  return BT_FAILURE;  // All children failed
}

// TODO: Const. arg.: children, period
PriorityNode* CreatePriorityNode(BTNode** children, u32 count, float period) {
  PriorityNode* node = malloc(sizeof(PriorityNode));
  node->children = children;
  node->childCount = count;
  node->period = period;
  node->lastExecutionTime = 0;
  node->base.tick = (BTStatus(*)(BTNode*))PriorityTick;
  return node;
}

// 16. WhileNode

// The WhileNode runs its child node
// as long as a specified condition is true.
// It effectively stops when the condition returns false.

// WhileNode allows for repeated execution of a child node
// based on a dynamic condition, making it useful for
// looping behaviors until a condition changes.

BTStatus WhileTick(WhileNode* node) {
  if (node->condition()) {
    return node->child->tick(node->child);  // Continue running the child if the condition is true
  }
  return BT_FAILURE;  // Stop if the condition is false
}

// TODO: Const. arg.: cond, name, node
WhileNode* CreateWhileNode(bool (*condition)(), BTNode* child) {
  WhileNode* node = malloc(sizeof(WhileNode));
  node->condition = condition;
  node->child = child;
  node->base.tick = (BTStatus(*)(BTNode*))WhileTick;
  return node;
}

// TODO: EventNode listens, wakes parent priority node, visits child
// TODO: EventLoop impl