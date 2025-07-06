# The OBC firmware repository v2 for the STM32F407-based custom board

This repository contains C and C++ code for the STM32F407-based OBC board, made with [PlatformIO IDE](https://platformio.org/platformio-ide) using the [Arduino framework](https://docs.platformio.org/en/latest/frameworks/arduino.html)

## Outline

The basic program structure is a `setup` function which is run once on startup and the a `loop` function which runs forever from then onwards. The loop will call a list of functions from dedicated code files which perform specific tasks, such as getting the current location from the GPS module onboard ADCS, reading voltages from EPS, or sending data via Comms. Each function should run for no longer than 100 ms, and there will be a watchdog which is petted after each function call exits.  

## Contributors

The [obc-tasks GitHub Project](https://github.com/orgs/guorbit/projects/9) is where contributors will see `Todo`, `In Progress`, and `Done` tasks. This page has an `Overview` tab showing all issues, and a `My Items` tab in which contributors can see the items assigned to them. If you have no assigned, open tasks then you may assign yourself to any unassigned task in the `Todo` column.  

If you have been assigned a task, then please move it to the `In Progress` column. Once a pull request to `main` has been merged, tasks will be moved to `Done`. 

Please keep `main.cpp` clean. Create new features in new files (`feature_name.cpp` and `feature_name.h`) and only add an `include` and the relevant function calls to run your feature to `main.cpp`.  

Nobody shall push to the `main` branch.  Feature requests and bug reports will be opened and individual feature or bug branches will be created to address these. [This page on the GitHub Docs](https://docs.github.com/en/issues/tracking-your-work-with-issues/using-issues/creating-a-branch-for-an-issue) describes how to make a branch for an issue.  Please create branches with the name `feature-featurename` or `bug-bugname`. 