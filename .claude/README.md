# Claude Code Configuration

This directory contains custom agents and commands for Claude Code that enhance code review, planning, and codebase analysis workflows.

## License

Portions of this configuration are derived from [humanlayer/humanlayer](https://github.com/humanlayer/humanlayer), licensed under the [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0).

## Installation & Setup

When installing this configuration in a new repository, you need to adapt the agents and commands to your specific codebase.

### Automated Setup

Use this prompt to have Claude automatically review and adapt the configuration files to your repository:

```
Please review all files in the .claude/agents/ and .claude/commands/ directories and update them to match this repository's structure and conventions. Specifically:

1. Update all example file paths to match the actual directory structure of this repository
2. Update the repository name in code-reviewer.md template from "payroll-agentic-tools" to the actual repo name
3. Review and update directory patterns in codebase-locator.md to match this repo's conventions
4. Update any technology-specific references to match the languages/frameworks used in this repo
5. Ensure all agents reference the correct directory paths for plans (.claude/ai/plans/), research (.claude/ai/research/), and reviews (.claude/ai/reviews/)
6. Update the test-runner.md agent to match the testing setup in this repository:
   - Identify the test framework(s) used (pytest, jest, go test, npm test, etc.)
   - Find the test directory structure and file patterns
   - Determine the correct command(s) to run tests
   - Update the test execution instructions accordingly

After making updates, provide a summary of what was changed.
```

### Manual Configuration Required

After running the automated setup, verify and manually configure:

1. **Repository Metadata**
   - Verify repository name is correct in `.claude/agents/code-reviewer.md` (line ~235)
   - Check that tags in the review template frontmatter are appropriate for your project

2. **Directory Structure**
   - The `.claude/ai/plans/`, `.claude/ai/research/`, and `.claude/ai/reviews/` directories will be created automatically when needed
   - You can optionally create them manually if you prefer to have them present from the start

3. **Testing Framework**
   - If not using pytest, update test-related sections in `code-reviewer.md`
   - Update test runner agent if using a different testing framework

### Additional Configuration Steps

5. **Code Reviewer Agent Customization**
   - Go through the code-reviewer agent and modify to suit your expected code standard
   - The current reviewer expects Python code - adjust to match your repository's language(s) and conventions

6. **Codebase Locator Agent Customization**
   - Update "common locations" examples to match your repository structure
   - Update "Refine by language/framework" section with your project's directory patterns
   - Update "common patterns to find" section with patterns relevant to your codebase

7. **Implement Plan Command**
   - Update test runner references to match your testing framework

## Agents Overview

- **code-reviewer**: Comprehensive code review agent that generates detailed review reports
- **codebase-analyzer**: Analyzes implementation details and traces data flow
- **codebase-locator**: Locates files and components by feature/topic
- **codebase-pattern-finder**: Finds similar implementations and design patterns
- **test-runner**: Executes tests and reports results
- **web-search-researcher**: Performs web research for technical questions
- **z-filter-agent**: Z-Filter (Main) variant implementation specialist — use for changes on the `main` branch
- **z-filter-xl-agent**: Z-Filter XL variant implementation specialist — use for changes on the `v2-xl` branch
- **z-filter-mini-agent**: Z-Filter Mini variant implementation specialist — use for changes on the `mini` branch

## Commands Overview

- **code-review**: Initiates a comprehensive code review process
- **create-plan**: Creates a structured implementation plan
- **implement-plan**: Implements code following a plan document
- **research-codebase**: Performs deep codebase research and analysis

## Directory Structure

```
.claude/
├── agents/          # Custom agent definitions
├── commands/        # Custom slash commands
├── ai/              # AI-generated documents (created automatically when needed)
│                    # Contains: plan-*.md, research-*.md, review-*.md files
└── README.md        # This file
```

## Usage Tips

- Use `/code-review` after completing significant code changes
- Create plans with `/create-plan` before starting complex features
- Use `/research-codebase` to understand existing implementations
- Run test-runner agent before requesting code reviews

## Variant Agent System

The Z-Filter project has three variant-specific agents that work with the `/implement-plan` command:

| Agent | Branch | Variant | Hierarchy |
|-------|--------|---------|-----------|
| `z-filter-agent` | `main` | Z-Filter (15 params) | Primary |
| `z-filter-xl-agent` | `v2-xl` | Z-FilterXL (24 params) | Second |
| `z-filter-mini-agent` | `mini` | Z-FilterMini (7 params) | Third |

When `/implement-plan` detects a multi-variant plan, it executes agents sequentially in hierarchy order: Main → XL → Mini. The Main agent's implementation serves as the reference that XL and Mini adapt from.

## Customization

All agents and commands can be customized by editing their respective markdown files. Each file contains:

- **Frontmatter**: Agent/command metadata and configuration
- **Instructions**: Detailed behavior and guidelines for the agent/command
- **Templates**: Output format templates and examples

Modify these to match your team's workflows and standards.
