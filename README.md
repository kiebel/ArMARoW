[ArMARoW](http://github.com/kiebel/ArMARoW)
===========================================

ArMARoW (Architecture for Medium Access control and Routing Protocols in Wireless
sensor networks) is a highly configurable radio stack for wireless sensor
networks on embedded systems based on the IEEE-802.15.4 physical layer. It does
not need an operating system and by employing C++ Template Meta Programing it
can be used to creates efficient, portable and reusable applications.

Key features of ArMARoW are:

    * Configurability
    * Portability
    * Adaptability
    * Hiding of heterogeneity by abstracting underlying layers
    * Deployment of modern software technologies

How to participate
------------------

ArMARow is under active development and anybody is welcome to participate. Feel
free to contact one of the developers listed below or go to the project page
that is used for bug tracking, feature requests, wiki and version control.

    - Project Maintainer: Thomas Kiebel (kiebel@ivs.cs.uni-magdeburg.de)
    - Main Developer: Thomas Kiebel (kiebel@ivs.cs.uni-magdeburg.de)
    - Developer: Christoph Steup (christoph.steup@student.ovgu.de)
    - Developer: Marcus Foerster (marcus.foerster@student.ovgu.de)
    - Developer: Karl Fessel (karl.fessel@student.ovgu.de)

You can obtain the sources of the project by downloading the provided tarball or
ZIP archive, or by checking out the project repository using either GIT or SVN.
Simply run one of the following commands:

    $ git clone git://github.com/kiebel/ArMARoW
    $ svn checkout svn://svnhub.com/kiebel/ArMARoW

Please apply the following astyle command to header and source files before
checkin to ensure that your source code corresponds to the coding style of the
project.

    $ astyle -aCNopKSV

Structure
---------

    armarow/
      doc/                  dokumentation of the ArMARoW project
        dox/                dokumentation not contained in header/sourcefiles
        images/             images used for dokumentation
      examples/             examples for the project
        application/        simple example programs (sniffer, repeater, aso.)
        unittest/           unit tests (unittest-phy, unittest-mac, unittest-rol)
      external/             external projects needed for ArMARoW
      include/              include files of the project
        armarow/            armarow specific include files
          mac/              medium access controll
          phy/              physical layer
            default/        default radio controller (<NAME>-rc.h, <NAME>-spec.h).
          rol/              routing layer
      platform/             make system and supported platforms (CC2420DB, ICRadioStick, ao.)
