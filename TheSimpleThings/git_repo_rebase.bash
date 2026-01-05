#!/bin/bash

#
# Die Frage kam auf bei den OpenSourceNerds, wie wir denn unsere Software aktualisieren
# da hab ich gleich mal mit dem Prompt rumgefragt "aktualisieren in linux distributionen"
# ist natürlich immer auch ein security Thema. 
# Wie hier beschrieben: https://linuxsecurity.com/howtos/learn-tips-and-tricks/upgrade-your-linux-distro
# 
# Aber darum gings bei der Diskussion nicht
# Eigentlich ging's darum, wie man seine privat geklonten git repositories auf Stand hält,
# mit integrity check, API Kompatibilität und dem ganzen Schnickschnack
# Naja. 
# Darum geht's hier.
# Mal schauen, was das wird
#

#
# Jetzt schauen wir mal, wo wir das reinloggen, was wir machen
#
GIT_REPO_LOG_FILE=~/git_repo_check_log_USER_$(whoami)_HOSTNAME_$(hostname).log

#
# verschiedene checks
#
GIT_REPO_STATISTICS=git_repo_statistics.bash
GIT_REPO_REQUIRES_REBASE=git_requires_rebase_check.bash
GIT_REPO_INTEGRITY_CHECK=git_repo_integrity_check.bash
GIT_REPO_DEPENDENCY_CHECK=git_repo_dependency_check.bash

#
# Reihenfolge der checks
#
EXECUTION_CHAIN=""
# Statistiken whatever
EXECUTION_CHAIN=$EXECUTION_CHAIN $GIT_REPO_STATISTICS
# Braucht der Klon nen rebase?
EXECUTION_CHAIN="$EXECUTION_CHAIN $GIT_REPO_REQUIRES_REBASE"
# Welche Abhängigkeiten hat das?
EXECUTION_CHAIN="$EXECUTION_CHAIN $GIT_REPO_DEPENDENCY_CHECK"
# Wie teste ich die Integrität?
EXECUTION_CHAIN="$EXECUTION_CHAIN $GIT_REPO_INTEGRITY_CHECK"

echo "Da kommen die Logs rein: " $GIT_REPO_LOG_FILE

#
# erstmal alle folders finden, die git repositories geklont enthalten
#
gitfolders=$(find . -name .git -a -type d)

for g in $gitfolders
do
  GIT_FOLDER=$(dirname $g)
  cd $GIT_FOLDER
  echo "git folder " $GIT_FOLDER " , git remote " $(git remote -v | awk '{remote[$1] = $2}END{for(i in remote){cnt++;res = res " " remote[i] " "}print res "(total remotes )"cnt}')
  # Was mache ich mit den integrity checks ? 
  for ex in $EXECUTION_CHAIN
  do 
    if [ -f $ex ]
    then
      echo Ausführung $ex für $(pwd) 
      $ex $(pwd) 
    else
      echo "Ich wollte $ex ausführen, finde es aber nicht"
    fi
  done
  # wieder zurück
  cd - >/dev/null
done

