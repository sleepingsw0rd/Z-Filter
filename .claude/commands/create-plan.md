# Implementation Plan

You are tasked with creating detailed implementation plans through an interactive, iterative process. You should be skeptical, thorough, and work collaboratively with the user to produce high-quality technical specifications.

**Output**: Implementation plan will be saved to `.claude/ai/plan-YYYY-MM-DD-description.md`

## Initial Response

When this command is invoked:

1. **Check if parameters were provided**:

   - If a file path or ticket reference was provided as a parameter, skip the default message
   - Immediately read any provided files FULLY
   - Begin the research process

2. **If no parameters provided**, respond with:

```
I'll help you create a detailed implementation plan for the Z-Filter audio plugin. Let me start by understanding what we're building.

Please provide:
1. Any relevant context, constraints, or specific requirements
3. Links to related research or previous implementations

I'll analyze this information and work with you to create a comprehensive plan.
```

Then wait for the user's input.

## Process Steps

### Step 1: Context Gathering & Initial Analysis

1. **Read all mentioned files immediately and FULLY**:

   - Research documents
   - Related implementation plans
   - Any JSON/data files mentioned
   - **IMPORTANT**: Use the Read tool WITHOUT limit/offset parameters to read entire files
   - **CRITICAL**: DO NOT spawn sub-tasks before reading these files yourself in the main context
   - **NEVER** read files partially - if a file is mentioned, read it completely

2. **Spawn initial research tasks to gather context**:
   Before asking the user any questions, use specialized agents to research in parallel:

   - Use the **codebase-locator** agent to find all files related to the task
   - Use the **codebase-analyzer** agent to understand how the current implementation works

   These agents will:

   - Find relevant source files, configs, and tests
   - Trace data flow and key functions
   - Return detailed explanations with file:line references

3. **Read all files identified by research tasks**:

   - After research tasks complete, read ALL files they identified as relevant
   - Read them FULLY into the main context
   - This ensures you have complete understanding before proceeding

4. **Analyze and verify understanding**:

   - Cross-reference the requirements with actual code
   - Identify any discrepancies or misunderstandings
   - Note assumptions that need verification
   - Determine true scope based on codebase reality

5. **Present informed understanding and focused questions**:

   ```
   Based on my research of the codebase, I understand we need to [accurate summary].

   I've found that:
   - [Current implementation detail with file:line reference]
   - [Relevant pattern or constraint discovered]
   - [Potential complexity or edge case identified]

   Questions that my research couldn't answer:
   - [Specific technical question that requires human judgment]
   - [Business logic clarification]
   - [Design preference that affects implementation]
   ```

   Only ask questions that you genuinely cannot answer through code investigation.

### Step 2: Research & Discovery

After getting initial clarifications:

1. **If the user corrects any misunderstanding**:

   - DO NOT just accept the correction
   - Spawn new research tasks to verify the correct information
   - Read the specific files/directories they mention
   - Only proceed once you've verified the facts yourself

2. **Create a research todo list** using TodoWrite to track exploration tasks

3. **Spawn parallel sub-tasks for comprehensive research**:

   - Create multiple Task agents to research different aspects concurrently
   - Use the right agent for each type of research:

   **For deeper investigation:**

   - **codebase-locator** - To find more specific files (e.g., "find all files that handle [specific component]")
   - **codebase-analyzer** - To understand implementation details (e.g., "analyze how [system] works")
   - **codebase-pattern-finder** - To find similar features we can model after

   Each agent knows how to:

   - Find the right files and code patterns
   - Identify conventions and patterns to follow
   - Look for integration points and dependencies
   - Return specific file:line references
   - Find tests and examples

4. **Wait for ALL sub-tasks to complete** before proceeding

5. **Present findings and design options**:

   ```
   Based on my research, here's what I found:

   **Current State:**
   - [Key discovery about existing code]
   - [Pattern or convention to follow]

   **Design Options:**
   1. [Option A] - [pros/cons]
   2. [Option B] - [pros/cons]

   **Open Questions:**
   - [Technical uncertainty]
   - [Design decision needed]

   Which approach aligns best with your vision?
   ```

### Step 3: Plan Structure Development

Once aligned on approach:

1. **Create initial plan outline**:

   ```
   Here's my proposed plan structure:

   ## Overview
   [1-2 sentence summary]

   ## Implementation Phases:
   1. [Phase name] - [what it accomplishes]
   2. [Phase name] - [what it accomplishes]
   3. [Phase name] - [what it accomplishes]

   Does this phasing make sense? Should I adjust the order or granularity?
   ```

2. **Get feedback on structure** before writing details

### Step 4: Detailed Plan Writing

After structure approval:

1. **Gather metadata for the plan document:**
   - Gather relevant metadata from git (commit hash, branch name, repository name, timestamps)
   - Filename: `.claude/ai/plan-YYYY-MM-DD-description.md`
     - Format: `plan-YYYY-MM-DD-description.md` where:
       - YYYY-MM-DD is today's date
       - description is a brief kebab-case description
     - Examples:
       - `plan-2025-10-03-improve-error-handling.md`
       - `plan-2025-10-03-add-authentication.md`

2. **Use this template structure with proper metadata:**

````markdown
---
date: [Current date and time with timezone in ISO format]
planner: Claude
git_commit: [Current commit hash]
branch: [Current branch name]
repository: [Current repository name]
plan_type: "implementation"
topic: "[Feature/Task Name]"
tags: [implementation, planning, relevant-component-names]
status: complete
last_updated: [Current date in YYYY-MM-DD format]
last_updated_by: Claude
---
# [Feature/Task Name] Implementation Plan

**Date**: [Current date and time with timezone]
**Planner**: Claude
**Git Commit**: [Current commit hash]
**Branch**: [Current branch name]
**Repository**: [Repository name]

## Overview

[Brief description of what we're implementing and why]

## Current State Analysis

[What exists now, what's missing, key constraints discovered]

## Desired End State

[A Specification of the desired end state after this plan is complete, and how to verify it]

### Key Discoveries:

- [Important finding with file:line reference]
- [Pattern to follow]
- [Constraint to work within]

## What We're NOT Doing

[Explicitly list out-of-scope items to prevent scope creep]

## Implementation Approach

[High-level strategy and reasoning]

## Phase 1: [Descriptive Name]

### Overview

[What this phase accomplishes]

### Changes Required:

#### 1. [Component/File Group]

**File**: `path/to/file.ext`
**Changes**: [Summary of changes]

```[language]
// Specific code to add/modify
```

### Success Criteria:

#### Automated Verification:

- [ ] CMake configures successfully: `cmake -B build -S .`
- [ ] Build succeeds without errors: `cmake --build build --config Release`
- [ ] No new compiler warnings introduced
- [ ] VST3 and Standalone targets build: `cmake --build build --config Release --target ZFilter_All`

#### Manual Verification:

- [ ] Feature works as expected when tested via UI
- [ ] Performance is acceptable under load
- [ ] Edge case handling verified manually
- [ ] No regressions in related features

---

## Phase 2: [Descriptive Name]

[Similar structure with both automated and manual success criteria...]

---

## Testing Strategy

### Unit Tests:

- [What to test]
- [Key edge cases]

### Integration Tests:

- [End-to-end scenarios]

### Manual Testing Steps:

1. [Specific step to verify feature]
2. [Another verification step]
3. [Edge case to test manually]

## Performance Considerations

[Any performance implications or optimizations needed]

## Migration Notes

[If applicable, how to handle existing data/systems]

## References

- Related research: `[relevant documentation]`
- Similar implementation: `[file:line]`
````

### Step 5: Save and Review

1. **Save the plan:**
   - Save the plan document to `.claude/ai/plan-YYYY-MM-DD-description.md`
   - Include the metadata from step 4.1 in the document

2. **Present the plan:**

   ```
   I've created the implementation plan at:
   `.claude/ai/plan-YYYY-MM-DD-description.md`

   Please review it and let me know:
   - Are the phases properly scoped?
   - Are the success criteria specific enough?
   - Any technical details that need adjustment?
   - Missing edge cases or considerations?
   ```

3. **Iterate based on feedback** - be ready to:

   - Add missing phases
   - Adjust technical approach
   - Clarify success criteria (both automated and manual)
   - Add/remove scope items

4. **Continue refining** until the user is satisfied

## Important Guidelines

1. **Be Skeptical**:

   - Question vague requirements
   - Identify potential issues early
   - Ask "why" and "what about"
   - Don't assume - verify with code

2. **Be Interactive**:

   - Don't write the full plan in one shot
   - Get buy-in at each major step
   - Allow course corrections
   - Work collaboratively

3. **Be Thorough**:

   - Read all context files COMPLETELY before planning
   - Research actual code patterns using parallel sub-tasks
   - Include specific file paths and line numbers
   - Write measurable success criteria with clear automated vs manual distinction

4. **Be Practical**:

   - Focus on incremental, testable changes
   - Consider migration and rollback
   - Think about edge cases
   - Include "what we're NOT doing"

5. **Track Progress**:

   - Use TodoWrite to track planning tasks
   - Update todos as you complete research
   - Mark planning tasks complete when done

6. **No Open Questions in Final Plan**:
   - If you encounter open questions during planning, STOP
   - Research or ask for clarification immediately
   - Do NOT write the plan with unresolved questions
   - The implementation plan must be complete and actionable
   - Every decision must be made before finalizing the plan

## Success Criteria Guidelines

**Always separate success criteria into two categories:**

1. **Automated Verification** (can be run by execution agents):

   - Commands that can be run: `cmake -B build -S .`, `cmake --build build --config Release`, etc.
   - Specific files that should exist
   - Code compilation with no warnings
   - Both VST3 and Standalone targets build successfully

2. **Manual Verification** (requires human testing):
   - UI/UX functionality
   - Performance under real conditions
   - Edge cases that are hard to automate
   - User acceptance criteria

**Format example:**

```markdown
### Success Criteria:

#### Automated Verification:

- [ ] CMake configures successfully: `cmake -B build -S .`
- [ ] Build succeeds without errors: `cmake --build build --config Release`
- [ ] No new compiler warnings introduced
- [ ] Both VST3 and Standalone targets build successfully

#### Manual Verification:

- [ ] Plugin loads correctly in a DAW (VST3 host)
- [ ] Standalone application launches and produces audio
- [ ] UI controls respond correctly and update parameters
- [ ] Audio processing sounds correct across all filter types
```

## Common Patterns

### For New DSP Features:

- Research existing filter/processing patterns in PluginProcessor.cpp
- Add parameters to APVTS layout in createParameterLayout()
- Implement DSP logic in processBlock()
- Add GUI controls in PluginEditor (knobs, buttons, LEDs)
- Wire up APVTS attachments and update LCD display

### For GUI Changes:

- Add component declarations in PluginEditor.h
- Create and position components in constructor and resized()
- Add APVTS attachments for parameter binding
- Update updateDisplay() for LCD/LED feedback
- Update paint() for labels and section headers

### For Refactoring:

- Document current behavior
- Plan incremental changes
- Verify build after each phase

## Sub-task Spawning Best Practices

When spawning research sub-tasks:

1. **Spawn multiple tasks in parallel** for efficiency
2. **Each task should be focused** on a specific area
3. **Provide detailed instructions** including:
   - Exactly what to search for
   - Which directories to focus on
   - What information to extract
   - Expected output format
4. **Be EXTREMELY specific about directories**:
   - Include the full path context in your prompts
5. **Specify read-only tools** to use
6. **Request specific file:line references** in responses
7. **Wait for all tasks to complete** before synthesizing
8. **Verify sub-task results**:
   - If a sub-task returns unexpected results, spawn follow-up tasks
   - Cross-check findings against the actual codebase
   - Don't accept results that seem incorrect

Example of spawning multiple tasks:

```python
# Spawn these tasks concurrently:
tasks = [
    Task("Research database schema", db_research_prompt),
    Task("Find API patterns", api_research_prompt),
    Task("Investigate UI components", ui_research_prompt),
    Task("Check test patterns", test_research_prompt)
]
```

## Example Interaction Flow

```
User: /create-plan
Assistant: I'll help you create a detailed implementation plan...

User: We need to improve the video analysis performance.
Assistant: Let me research the codebase to understand the current implementation...

[Researches codebase]

Based on my research, I understand we need to optimize video processing speed. Before I start planning, I have some questions...

[Interactive process continues...]
```