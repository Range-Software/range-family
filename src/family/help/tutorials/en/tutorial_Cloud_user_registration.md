# Cloud - User registration

Registered **Range Cloud** users may authenticate using CA signed client certificate or by using temporary one-time _authentication token_. This guide will demonstrate how to register new user on **Range Cloud**. The whole registration process consists of following steps:
1. Creation of new _user account_
2. Generation of temporary one-time _authentication token_
3. Generation of _Certificate Signing Request (CSR)_
4. Sending _CSR_ to **Range Cloud** using newly created _user account_ and generated _authentication token_
5. Fetching _CA signed client certificate_ from **Range Cloud** and configuring setting it for future usage

By using _Cloud session manager_ this whole process is simplified to few clicks.

## 1. Confirm or change user settings

**Menu:** _File -> Application settings_

First make sure that user personal information is set.

![User settings](image-User_settings.png)

## 2. Open Cloud session manager

**Menu:** _Cloud -> Cloud session manager_

Keep all values as they are and click _Request new certificate_ button.

![Cloud session](image-Cloud_session.png)

## 3. Generate Certificate Signing Request (CSR)

In newly open _OpenSSL certificate signing request_ dialog window fill in all missing _Certificate subject fields_ and click _Generate certificate signing request_ button.

**Note:** _CN field_ must contain an email address which will be used as your **Range Cloud** _user name_.

![Cloud CSR - generate](image-Cloud_csr_generate.png)

## 4. Register new user

Once _CSR_ is generated its content will be shown in previously empty text area. To continue click _Register new user_ button which will request to create a new _user name_ along with _authentication token_ on **Range Cloud**.

![Cloud CSR - user registration](image-Cloud_csr_user_registration.png)

Once new _user account_ is successfully created a confirmation dialog window will appear containing user details. After clicking _OK_ button _CSR_ will be send to **Range Cloud**.

![Cloud CSR - user registered](image-Cloud_csr_user_registered.png)

## 5. Sign user certificate

After _CSR_ is successfully processed and _client certificate_ is signed by **Range CA** a dialog window will appear offering to apply newly generated key ans signed certificate. Accept it.

![Cloud CSR - certificate signed](image-Cloud_csr_cert_signed.png)

## 6. Accept generated key and signed certificate

_Cloud session manager_ dialog window now shows correct _Private key_ and _Certificate_ file names. Clicking _OK_ button will save it for the future use.

![Cloud session accept](image-Cloud_session_accept.png)
