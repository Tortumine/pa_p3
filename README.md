# Programmation avancée Projet 3

[PDF](doc/p3.pdf)

## Mise en page automatique d'une bande dessinée

## DEADLINE: Vendredi 22 décembre 2017 à 23h59

## TODO

* Nothing
* Yet

## git cheat sheet

* Raccourci pour commit:  __git commit -m "[message descriptif]"__
* Sauver les modifications courantes dans une pile (s'utilse avant un __pull__):  __git stash__
* Récupérer depuis la pile:  __git stash pop__
* Supprimer la dernière sauvegarde: __git stash drop__
* Ajouter le suivit des autres branches: __git fetch --all__
* Pull toutes les branches: __git pull --all__
* Aller sur une branche et la suivre: __git checkout -b LocalName origin/nombranchedistante__
* Fusionner une brnache avec la branche actuelle: __git merge nombranche__

## Structure du projet

Afin de bien organiser le tout et éviter les problèmes de merge autant que possible, je préconise l'utilisation du patern suivant
![Modèle git](doc/img/git-model.png)

* La branche __master__ heberge le code opérationnel.
* La branche __develop__ heberge le code avec les features fonctionnelles mais en cours/attente d'intégration au reste de l'application.
* Les branches __feature-name__ sont le travail en cours de chaqu'un (plus simple à s'y retrouver qu'avec _alex-branche1_ ou _antoine2_)

Note:

* Une branche de test peut être interessante pour exécuter des tests complets avant de _merge_ sur la branche __master__ et sans que ça gêne la branche __develop__.
* Comme sur la jolie image, une branche __hotfixes__ permeterai de corriger des bugs soudains sans mettre en péril le reste du code.
* La branche master sera protégée contre les _git push force_ et les _merge_ qui induiraient des conflits.