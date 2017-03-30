Contribution Guide
==================

This document describes some points about contribution process.

Commit with Issue
-------------

If you fixed or added something useful to the project, you would first make sure you have an issue to check in code.

> 每次commit之前，在commit message必须使用 #ISSUE number, 如果没有issue，可以使用当前 milestone的 backlog issue number. 如果是自动的merge code，则不需要提供issue number.

For example - 
```
git add --all
git commit -m "#11 add a dialog for confromation"
```

Bugs
----

If you found an error, mistype or any other flawback in the project, please report about it using [issues](https://github.com/Samurais/xfy-node/issues).
The more details you provide, the easier it can be reproduced and the faster can be fixed.

Features
--------

It you've got an idea about a new feature, it's most likely that you have do implement it on your own.
If you cannot implement the feature, but it is very important, you can add a task in [issues](https://github.com/Samurais/xfy-node/issues), but expect it be declined by the maintainer.

npm mirror in taobao
---------
淘宝 NPM 镜像
https://npm.taobao.org/
RubyGems 镜像
https://ruby.taobao.org/
