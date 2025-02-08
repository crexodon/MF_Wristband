<script setup lang="ts">
import { OnyxButton, OnyxForm, OnyxInput, OnyxCheckbox, useToast } from 'sit-onyx';
import { RegisterService } from '@/services/register.ts';
import { computed, ref, useTemplateRef } from 'vue';
import ScanInput from '@/components/ScanInput.vue';

const toast = useToast();
console.log(import.meta.env.VITE_API_URL);
const registerSerice = new RegisterService(import.meta.env.VITE_API_URL);
const loading = ref<boolean>(false);
const tagID = ref<string>();
const email = ref<string>('');
const dataProtection = ref<boolean>(false);
const participation = ref<boolean>(false);
const emailInputRef = useTemplateRef('mail');
const showMailStep = computed<boolean>(() => !!tagID.value);
const showSubmitStep = computed<boolean>(
  () =>
    !!tagID.value &&
    !!email.value &&
    email.value?.includes('@') &&
    participation.value &&
    dataProtection.value,
);
const showLegalStep = computed<boolean>(
  () => !!tagID.value && !!email.value && email.value?.includes('@'),
);

const wrapLoading = async (f: () => Promise<void>) => {
  loading.value = true;
  try {
    await f();
  } finally {
    loading.value = false;
  }
};

const onTagValue = (val: string) => {
  tagID.value = val;
  setTimeout(() => {
    emailInputRef.value?.input?.focus();
  }, 50);
};

const onRegister = async (e: Event) => {
  e.preventDefault();

  wrapLoading(async () => {
    if (tagID.value === undefined) {
      toast.show({
        headline: 'Keine Armband-ID eingegeben',
        description: 'Benutze den Scanner um dein Armband zu scannen',
        color: 'danger',
      });

      loading.value = false;

      return;
    }

    const response = await registerSerice.registerEmail(tagID.value, email.value);
    if (response.status !== 'success') {
      if (response.error === 'Participant not found') {
        toast.show({
          headline: 'Nicht registriert',
          description:
            'Das Armband wurde nicht gefunden. Hast du es korrekt am Anfang registriert?',
          color: 'warning',
          duration: 10000,
        });
      } else if (response.error === 'Not all stations') {
        toast.show({
          headline: 'Du bist noch nicht fertig',
          description:
            'Du hast leider noch nicht alle Stationen abgeschlossen. Bitte schau nochmal vorbei, wenn du fertig bist',
          color: 'warning',
          duration: 10000,
        });
      } else {
        toast.show({
          headline: 'Fehler',
          description:
            'Es ist leider etwas schief gegangen. Bitte probiere es nochmal. Falls das immer noch nicht funktioniert, wende dich bitte an einen der Mitarbeitenden.',
          color: 'danger',
          duration: 10000,
        });
      }

      participation.value = false;
      dataProtection.value = false;
      email.value = '';
      tagID.value = '';

      return;
    }

    const registerTime = new Date(response.registered_at);
    const finishTime = new Date(response.finished_at);
    let diff = (finishTime.getTime() - registerTime.getTime()) / 1000;
    const hours = Math.floor(diff / 3600);
    diff %= 3600;
    const minutes = Math.floor(diff / 60);
    diff %= 60;
    const seconds = Math.floor(diff);

    toast.show({
      headline: 'E-Mail erfolgreich hinterlegt',
      description: `Du hast ${hours}h ${minutes}m ${seconds}s gebraucht um alle Station abzuschließen.`,
      duration: 10000,
    });
    participation.value = false;
    dataProtection.value = false;
    email.value = '';
    tagID.value = '';
  });
};
</script>

<template>
  <main class="onyx-grid-container">
    <div class="onyx-grid">
      <h1 class="onyx-grid-span-full">Am Gewinnspiel teilnehmen</h1>
      <OnyxForm class="onyx-grid-span-6 form">
        <h2>Registrierung</h2>
        <ul class="step-list">
          <li class="step">
            <h3>Schritt 1: Armband scannen</h3>
            <ScanInput :expected-length="14" @value="onTagValue" v-model="tagID" />
          </li>
          <li class="step" v-if="showMailStep">
            <h3>Schritt 2: E-Mail Addresse angeben</h3>
            <OnyxInput label="E-Mail" type="email" v-model="email" ref="mail"></OnyxInput>
          </li>
          <li class="step" v-if="showLegalStep">
            <h3>Schritt 3: Datenschutz und Teilnahmebedingungen</h3>
            <OnyxCheckbox
              label="Ich habe die Teilnahmebedingungen gelesen und stimme diesen zu"
              v-model="participation"
              value="participation"
              required
            />
            <OnyxCheckbox
              label="Ich habe die Datenschutzbestimmungen gelesen und stimme diesen zu"
              v-model="dataProtection"
              value="dataprotection"
              required
            />
          </li>
          <li class="step" v-if="showSubmitStep">
            <h3>Schritt 4: Registrieren</h3>
            <OnyxButton
              class="onyx-grid-span-4"
              label="Registrieren"
              type="submit"
              @click="onRegister"
              :loading
            ></OnyxButton>
          </li>
        </ul>
      </OnyxForm>
      <div class="onyx-gid-span-1"></div>
      <div class="onyx-grid-span-5 information">
        <h2>1. Teilnahmebedingungen</h2>
        <p>Die Teilnahme am Gewinnspiel ist freiwillig und kostenlos.</p>
        <p>Teilnahmeberechtigt sind alle Besucher des Maker Faire Heilbronn ab 14 Jahren.</p>
        <p>
          Zur Teilnahme müssen alle drei Schnitzeljagd-Stationen gefunden und die RFID-Papierarmband
          gescannt werden.
        </p>
        <p>
          Nach Abschluss der Schnitzeljagd muss die E-Mail-Adresse an der Computerstation neben dem
          Koala eingegeben werden.
        </p>
        <p>Jeder Teilnehmer darf nur einmal am Gewinnspiel teilnehmen.</p>
        <p>Die Gewinner werden per Zufallsprinzip ermittelt.</p>
        <p>Die Teilnahme am Gewinnspiel setzt die Zustimmung zur Datenschutzerklärung voraus.</p>
        <h2>2. Gewinne</h2>
        <p>
          Es gibt insgesamt drei Gewinner, die jeweils einen 30€ Gutschein für die experimenta
          erhalten.
        </p>
        <p>Die Gewinner werden per E-Mail benachrichtigt.</p>
        <p>
          Falls ein Gewinner nicht innerhalb von 7 Tagen auf die Gewinnbenachrichtigung reagiert,
          wird ein neuer Gewinner ausgelost.
        </p>
        <p>Eine Barauszahlung oder ein Umtausch der Gewinne ist nicht möglich.</p>

        <h2>3. Datenschutz</h2>
        <p>
          Die im Rahmen des Gewinnspiels erhobenen personenbezogenen Daten (E-Mail-Adresse) werden
          ausschließlich zur Durchführung des Gewinnspiels verwendet.
        </p>
        <p>
          Die Daten werden nicht an Dritte weitergegeben und nach Abschluss des Gewinnspiels
          gelöscht.
        </p>
        <p>Die Teilnehmer haben das Recht auf Auskunft, Berichtigung und Löschung ihrer Daten.</p>
        <p>Verantwortlich für die Datenverarbeitung ist experimenta gGmbH.</p>
        <p>
          Bei Fragen zum Datenschutz kann sich der Teilnehmer per E-Mail an
          datenschutz@experimenta.science.
        </p>
      </div>
    </div>
  </main>
</template>

<style scoped>
.form {
  display: flex;
  flex-direction: column;
  gap: 1rem;
}

.step-list {
  margin-left: 32px;
  border-left: 5px solid #ddd;

  list-style-type: none;
  position: relative;
}

.step {
  padding: 15px 0;
  position: relative;
  height: 128px;
}

.step::before {
  content: '';
  display: block;
  width: 15px;
  height: 15px;
  border-radius: 30px;
  background-color: #999;
  position: absolute;
  margin-left: 20px;
  top: 24px;
  left: -70px;
}

.information h2:not(:first-of-type) {
  margin-top: 16px;
}
</style>
