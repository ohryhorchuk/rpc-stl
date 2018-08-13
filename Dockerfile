FROM centos:7

COPY docker/.ssh/* /root/.ssh/

COPY docker/etc /etc

RUN chown root:root /root/.ssh/* && chmod 600 /root/.ssh/*

RUN yum -y upgrade

RUN yum install -y centos-release-scl && yum install -y make git devtoolset-7 cmake rpm-build