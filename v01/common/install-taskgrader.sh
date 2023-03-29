#!/bin/bash

# Ce script installe le taskgrader dans le dossier courant.
# https://github.com/France-ioi/taskgrader

cd "$( dirname "${BASH_SOURCE[0]}" )"
DIR="`pwd`"

# Le dossier du taskgrader existe déjà
if [ -d "$DIR/taskgrader" ]
then
  if [ -d "$DIR/taskgrader/.git" ]
  then
    echo "Le taskgrader semble déjà installé dans le dossier:"
    echo "  '$DIR/taskgrader'"
    echo
    read -p "Voulez-vous le mettre à jour ? [Y/n] " -n 1 -r
    echo
    if ! [[ $REPLY =~ ^[Nn]$ ]]
    then
      echo "Mise à jour du taskgrader via 'git pull'..."
      cd taskgrader
      if git pull
      then
        # On copie le module bottle
        cp -p ../default-taskgrader/bottle.py tools/testSelect/

        # Mise à jour de la documentation
        if [ -d "$DIR/taskgrader-docs" ]
        then
          echo "Mise à jour de la documentation..."
          cd ../taskgrader-docs
          git pull
          cd ../taskgrader
        fi

        exit 0
      else
        echo "Échec de la mise à jour."
      fi
    fi
    # Soit l'utilisateur a répondu non, soit le git pull a échoué
    echo
    echo "Pour réinstaller le taskgrader à partir de zéro, veuillez supprimer ou déplacer"
    echo "le dossier '$DIR/taskgrader'."
  else
    echo "Le dossier '$DIR/taskgrader'"
    echo "existe déjà, mais ne semble pas être une installation du repository git; mise à"
    echo "jour impossible."
    echo "Pour utiliser ce script, veuillez supprimer ou déplacer ce dossier."
  fi
  exit 0
fi

echo "Ce script va installer le taskgrader dans le dossier:"
echo "  '$DIR/taskgrader'"
echo "puis lancer le script 'install.sh' du taskgrader."
echo
echo "En cas d'erreur d'installation, vérifiez la sortie, puis réexécutez le script"
echo "'install.sh' dans le dossier du taskgrader."
echo
echo "Veuillez aussi lire la documentation disponible dans le dossier"
echo "'taskgrader-docs' une fois installé, ou alors à l'adresse"
echo "http://france-ioi.github.io/taskgrader/ ."
echo "Elle couvre entre autres l'installation des dépendances, les spécificités"
echo "sur certains systèmes d'exploitation et l'écriture de tasks."
echo

read -p "Continuer ? [y/N] " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]
then
  echo
  # Récupération du repository du taskgrader
  if git clone https://github.com/France-ioi/taskgrader
  then
    echo
    echo "Récupération réussie."

    if ! [ -d "$DIR/taskgrader-docs" ]
    then
      echo "Récupération de la documentation du taskgrader..."
      if git clone -b gh-pages https://github.com/France-ioi/taskgrader taskgrader-docs
      then
        echo "Documentation du taskgrader installée dans le dossier 'taskgrader-docs'."
      else
        echo "Échec de la récupération de la documentation du taskgrader."
        echo "Vous pouvez y accéder en ligne à l'adresse"
        echo "http://france-ioi.github.io/taskgrader/ ."
      fi
      echo
    fi
    echo "Installation du fichier de config France-IOI pour genJson..."
    if [ -f taskgrader/tools/genJson/config.py ]
    then
      mv taskgrader/tools/genJson/config.py taskgrader/tools/genJson/config.py.old
    fi
    cp default-taskgrader/genJson-config.py taskgrader/tools/genJson/config.py
    sed -i "s|^CFG_ROOTDIR.*$|CFG_ROOTDIR=\"`dirname "$DIR"`/\"|" taskgrader/tools/genJson/config.py

    # On copie le module bottle
    cp -p default-taskgrader/bottle.py taskgrader/tools/testSelect/

    # On lance le script d'install du taskgrader
    echo "Lancement de 'taskgrader/install.sh'..."
    echo
    cd taskgrader
    ./install.py
  else
    echo "Erreur lors de la récupération du taskgrader depuis"
    echo "'https://github.com/France-ioi/taskgrader'."
    exit 1
  fi
else
  echo "Annulation."
  exit 0
fi
