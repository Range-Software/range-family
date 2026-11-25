# Cloud - Registrazione utente

Gli utenti registrati di **Range Cloud** possono autenticarsi con un certificato client firmato dalla CA oppure con un *token di autenticazione* temporaneo monouso. Questa guida mostra come registrare un nuovo utente su **Range Cloud**. L'intero processo di registrazione è composto dai seguenti passi:
1. Creazione di un nuovo *account utente*
2. Generazione di un *token di autenticazione* temporaneo monouso
3. Generazione di una *richiesta di firma del certificato (CSR)*
4. Invio della *CSR* a **Range Cloud** con l'*account utente* appena creato e il *token di autenticazione* generato
5. Recupero del *certificato client firmato dalla CA* da **Range Cloud** e sua configurazione per l'uso futuro

Con il *Gestore sessioni Cloud* l'intero processo si riduce a pochi clic.

## 1. Confermare o modificare le impostazioni utente

**Menu:** *File -> Impostazioni dell'applicazione*

Per prima cosa assicurarsi che i propri dati personali siano impostati.

![Impostazioni utente](image-User_settings.png)

## 2. Aprire il Gestore sessioni Cloud

**Menu:** *Cloud -> Gestore sessioni Cloud*

Lasciare tutti i valori invariati e fare clic sul pulsante *Richiedi nuovo certificato*.

![Sessione Cloud](image-Cloud_session.png)

## 3. Generare la richiesta di firma del certificato (CSR)

Nella finestra di dialogo *Richiesta di firma del certificato OpenSSL* appena aperta, compilare tutti i campi mancanti del soggetto del certificato e fare clic sul pulsante *Genera richiesta di firma del certificato*.

**Nota:** Il *campo CN* deve contenere un indirizzo e-mail che verrà usato come *nome utente* su **Range Cloud**.

![Cloud CSR - generazione](image-Cloud_csr_generate.png)

## 4. Registrare un nuovo utente

Una volta generata la *CSR*, il suo contenuto viene mostrato nell'area di testo prima vuota. Per continuare, fare clic sul pulsante *Registra nuovo utente*, che richiederà a **Range Cloud** la creazione di un nuovo *nome utente* insieme a un *token di autenticazione*.

![Cloud CSR - registrazione utente](image-Cloud_csr_user_registration.png)

Una volta creato il nuovo *account utente*, compare una finestra di conferma con i dati dell'utente. Dopo aver fatto clic sul pulsante *OK*, la *CSR* viene inviata a **Range Cloud**.

![Cloud CSR - utente registrato](image-Cloud_csr_user_registered.png)

## 5. Firma del certificato utente

Dopo che la *CSR* è stata elaborata e il *certificato client* è stato firmato da **Range CA**, compare una finestra di dialogo che propone di applicare la chiave appena generata e il certificato firmato. Accettarla.

![Cloud CSR - certificato firmato](image-Cloud_csr_cert_signed.png)

## 6. Accettare la chiave generata e il certificato firmato

La finestra di dialogo *Gestore sessioni Cloud* mostra ora i nomi di file corretti per *Chiave privata* e *Certificato*. Facendo clic sul pulsante *OK* vengono salvati per l'uso futuro.

![Accettazione della sessione Cloud](image-Cloud_session_accept.png)
