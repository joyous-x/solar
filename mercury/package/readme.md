# Runtime Environment

## Architecture
```mermaid
graph LR
A[user] -->|request| B(nginx)
A[user] -->|request| C(gin)

B -.-> |service| C[gin]
B --> |static| D[home]
B --> |static| E[blog]
B --> |static| F[.etc]
D -.-> E[blog]
D -.-> F[.etc]
C--> K[service: go]
C--> L[service: python]
```

## Directory Structure
```
/data/www/:
    ├── envs
    │   ├── biz
    │   │   ├── docker-compose.biz.yaml
    │   │   └── dockfile.biz-server
    │   ├── crts
    │   │   ├── domain.key
    │   │   └── domain_bundle.crt
    │   ├── env
    │   │   ├── docker-compose.env.yaml
    │   │   ├── nginx.conf
    │   │   └── nginx.wordpress.conf
    │   └── wordpress
    |       └── ...
    ├── myapp
    │   ├── blog
    |   │   └── ...
    │   ├── krotas
    |   │   └── ...
    │   └── rsc
    │       └── image
    │           ├── ...
    |           └── avatar.png 
    ├── eploy.sh
    └── readme.md
```

## Package
使用脚本 ``` env_scripts/makefile ``` 完成打包

- cmdline
    + ``` cd env_scripts; make env=dev ```
- features
    + generate packages for deploy
    + generate config files with private value


## Deploy
### Run redis+mysql+nginx

### Run biz
