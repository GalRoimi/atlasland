#!/usr/bin/env python3

import argparse

DOCKERFILE_TEMPLATE = """FROM ubuntu:20.04
ENV DEBIAN_FRONTEND=noninteractive
RUN sed -Ei 's/^# deb-src /deb-src /' /etc/apt/sources.list

RUN yes | unminimize

RUN apt-get update && apt-get install -y apt-utils openssh-server man-db manpages-posix
RUN apt-get install -y make git qemu-system-arm gcc-arm-none-eabi gdb-multiarch cgdb socat python3.7 python3-pip cmatrix
RUN apt-get install -y nano vim screen
RUN pip3 install pyserial

{USERS}

RUN mkdir /var/run/sshd
RUN sed -i 's/PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config
RUN sed 's@session\\s*required\\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd

EXPOSE 22

CMD ["/usr/sbin/sshd", "-D"]"""

USER_ADD_CMDS_TEMPLATE = """RUN useradd -ms /bin/bash {USER}
RUN echo '{USER}:{PASS}' | chpasswd
RUN touch /home/{USER}/.hushlogin
RUN echo "" >> /home/{USER}/.profile
RUN echo "if [ -f \\"\$HOME/.loginrc\\" ]; then\\n    . \\"\$HOME/.loginrc\\"\\nfi" >> /home/{USER}/.profile
RUN echo "" >> /home/{USER}/.profile
RUN echo "cat .welcome" >> /home/{USER}/.profile
RUN echo "export QEMU_AUDIO_DRV=none" >> /home/{USER}/.bashrc
COPY {USER}/ /home/{USER}/
RUN chmod -R o-rx /home/{USER}/
RUN chown -R {USER}:{USER} /home/{USER}/
"""

def user_pass(s):
    try:
        user, password = map(str, s.split(','))
        return user, password
    except:
        raise argparse.ArgumentTypeError("must be user,pass")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--users', help="users list", type=user_pass, nargs='+')

    args = parser.parse_args()

    users = []
    for user, password in args.users:
        users.append(USER_ADD_CMDS_TEMPLATE.format(USER=user, PASS=password))

    print(DOCKERFILE_TEMPLATE.format(USERS='\n'.join(users)))

if __name__ == "__main__":
    main()
