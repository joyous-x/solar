#!/bin/bash

#
#   crontab:
#       */15 * * * * sh /data/www/deploy.sh >> /data/www/temp/deploy.log 2>&1 
#

set +x

WS=/data/www
TAR=${WS}/temp/myapp_from_github.tar.gz
Timestamp=`date +"%Y%m%d%H%M%S"`

## check file exists
if [ ! -e "${TAR}" ]; then
    echo "${Timestamp} : ${TAR} not exists ~"
    exit
fi

## extract files
tar_extracted=${WS}/temp/myapp_from_github
rm -rf ${tar_extracted} && mkdir ${tar_extracted} 
tar -zxvf ${TAR} -C ${tar_extracted}
if [ ! -d "${tar_extracted}" ];then
    echo "${Timestamp} : tar_extracted not exists, tar -zxvf ${TAR} ${tar_extracted}"
    exit
fi

## do replace
rm -rf ${WS}/temp/myapp.blog.bak
if [ -d "${WS}/myapp/blog" ];then
    mv ${WS}/myapp/blog ${WS}/temp/blog.bak
fi
mv ${tar_extracted} ${WS}/myapp/blog
mv ${TAR} ${TAR}.${Timestamp}

## restart
#cd ./package && docker-compose rm -f && docker-compose up --build
/usr/local/bin/docker-compose -f ${WS}/envs/env/docker-compose.env.yaml restart nginx
if [ $? -eq 0 ]; then
    echo "`date +"%Y%m%d.%H-%M-%S"` : deploy ${TAR} success, and rename to ${TAR}.${Timestamp}"
else
    echo "`date +"%Y%m%d.%H-%M-%S"` : deploy ${TAR} failed, but rename to ${TAR}.${Timestamp}"
fi