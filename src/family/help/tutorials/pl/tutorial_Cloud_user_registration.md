# Cloud - Rejestracja użytkownika

Zarejestrowani użytkownicy **Range Cloud** mogą uwierzytelniać się za pomocą certyfikatu klienta podpisanego przez CA lub tymczasowego jednorazowego *tokenu uwierzytelniającego*. Ten przewodnik pokazuje, jak zarejestrować nowego użytkownika w **Range Cloud**. Cały proces rejestracji składa się z następujących kroków:
1. Utworzenie nowego *konta użytkownika*
2. Wygenerowanie tymczasowego jednorazowego *tokenu uwierzytelniającego*
3. Wygenerowanie *żądania podpisania certyfikatu (CSR)*
4. Wysłanie *CSR* do **Range Cloud** przy użyciu nowo utworzonego *konta użytkownika* i wygenerowanego *tokenu uwierzytelniającego*
5. Pobranie *certyfikatu klienta podpisanego przez CA* z **Range Cloud** i skonfigurowanie go do dalszego użytku

Dzięki *Menedżerowi sesji Cloud* cały ten proces sprowadza się do kilku kliknięć.

## 1. Potwierdź lub zmień ustawienia użytkownika

**Menu:** *Plik -> Ustawienia aplikacji*

Najpierw upewnij się, że Twoje dane osobowe są ustawione.

![Ustawienia użytkownika](image-User_settings.png)

## 2. Otwórz Menedżera sesji Cloud

**Menu:** *Cloud -> Menedżer sesji Cloud*

Pozostaw wszystkie wartości bez zmian i kliknij przycisk *Zażądaj nowego certyfikatu*.

![Sesja Cloud](image-Cloud_session.png)

## 3. Wygeneruj żądanie podpisania certyfikatu (CSR)

W nowo otwartym oknie dialogowym *Żądanie podpisania certyfikatu OpenSSL* wypełnij wszystkie brakujące pola podmiotu certyfikatu i kliknij przycisk *Generuj żądanie podpisania certyfikatu*.

**Uwaga:** *Pole CN* musi zawierać adres e-mail, który będzie używany jako Twoja *nazwa użytkownika* w **Range Cloud**.

![Cloud CSR - generowanie](image-Cloud_csr_generate.png)

## 4. Zarejestruj nowego użytkownika

Po wygenerowaniu *CSR* jego treść zostanie wyświetlona w wcześniej pustym polu tekstowym. Aby kontynuować, kliknij przycisk *Zarejestruj nowego użytkownika*, który poprosi **Range Cloud** o utworzenie nowej *nazwy użytkownika* wraz z *tokenem uwierzytelniającym*.

![Cloud CSR - rejestracja użytkownika](image-Cloud_csr_user_registration.png)

Po pomyślnym utworzeniu nowego *konta użytkownika* pojawi się okno potwierdzenia z danymi użytkownika. Po kliknięciu przycisku *OK* *CSR* zostanie wysłane do **Range Cloud**.

![Cloud CSR - użytkownik zarejestrowany](image-Cloud_csr_user_registered.png)

## 5. Podpisanie certyfikatu użytkownika

Po pomyślnym przetworzeniu *CSR* i podpisaniu *certyfikatu klienta* przez **Range CA** pojawi się okno dialogowe proponujące zastosowanie nowo wygenerowanego klucza i podpisanego certyfikatu. Zaakceptuj je.

![Cloud CSR - certyfikat podpisany](image-Cloud_csr_cert_signed.png)

## 6. Zaakceptuj wygenerowany klucz i podpisany certyfikat

Okno dialogowe *Menedżer sesji Cloud* pokazuje teraz prawidłowe nazwy plików *Klucz prywatny* i *Certyfikat*. Kliknięcie przycisku *OK* zapisze je do dalszego użytku.

![Akceptacja sesji Cloud](image-Cloud_session_accept.png)
