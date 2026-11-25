# Cloud - Enregistrement d'un utilisateur

Les utilisateurs enregistrés de **Range Cloud** peuvent s'authentifier à l'aide d'un certificat client signé par la CA ou d'un *jeton d'authentification* temporaire à usage unique. Ce guide montre comment enregistrer un nouvel utilisateur sur **Range Cloud**. L'ensemble du processus d'enregistrement comprend les étapes suivantes :
1. Création d'un nouveau *compte utilisateur*
2. Génération d'un *jeton d'authentification* temporaire à usage unique
3. Génération d'une *demande de signature de certificat (CSR)*
4. Envoi de la *CSR* à **Range Cloud** avec le *compte utilisateur* nouvellement créé et le *jeton d'authentification* généré
5. Récupération du *certificat client signé par la CA* depuis **Range Cloud** et sa configuration pour une utilisation ultérieure

Grâce au *Gestionnaire de sessions Cloud*, tout ce processus se résume à quelques clics.

## 1. Confirmer ou modifier les paramètres utilisateur

**Menu :** *Fichier -> Paramètres de l'application*

Assurez-vous d'abord que vos informations personnelles sont renseignées.

![Paramètres utilisateur](image-User_settings.png)

## 2. Ouvrir le Gestionnaire de sessions Cloud

**Menu :** *Cloud -> Gestionnaire de sessions Cloud*

Conservez toutes les valeurs telles quelles et cliquez sur le bouton *Demander un nouveau certificat*.

![Session Cloud](image-Cloud_session.png)

## 3. Générer la demande de signature de certificat (CSR)

Dans la boîte de dialogue *Demande de signature de certificat OpenSSL* qui s'ouvre, remplissez tous les champs manquants du sujet du certificat et cliquez sur le bouton *Générer une demande de signature de certificat*.

**Remarque :** Le *champ CN* doit contenir une adresse e-mail qui servira de *nom d'utilisateur* sur **Range Cloud**.

![Cloud CSR - génération](image-Cloud_csr_generate.png)

## 4. Enregistrer un nouvel utilisateur

Une fois la *CSR* générée, son contenu s'affiche dans la zone de texte jusque-là vide. Pour continuer, cliquez sur le bouton *Enregistrer un nouvel utilisateur*, qui demandera à **Range Cloud** de créer un nouveau *nom d'utilisateur* ainsi qu'un *jeton d'authentification*.

![Cloud CSR - enregistrement de l'utilisateur](image-Cloud_csr_user_registration.png)

Une fois le nouveau *compte utilisateur* créé, une boîte de dialogue de confirmation contenant les informations de l'utilisateur apparaît. Après avoir cliqué sur le bouton *OK*, la *CSR* est envoyée à **Range Cloud**.

![Cloud CSR - utilisateur enregistré](image-Cloud_csr_user_registered.png)

## 5. Signature du certificat utilisateur

Une fois la *CSR* traitée et le *certificat client* signé par **Range CA**, une boîte de dialogue propose d'appliquer la clé nouvellement générée et le certificat signé. Acceptez-la.

![Cloud CSR - certificat signé](image-Cloud_csr_cert_signed.png)

## 6. Accepter la clé générée et le certificat signé

La boîte de dialogue *Gestionnaire de sessions Cloud* affiche maintenant les bons noms de fichiers pour *Clé privée* et *Certificat*. Cliquer sur le bouton *OK* les enregistre pour une utilisation ultérieure.

![Accepter la session Cloud](image-Cloud_session_accept.png)
