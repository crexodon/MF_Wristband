import './assets/main.css';

import { createApp } from 'vue';
import { createPinia } from 'pinia';
import { createOnyx } from 'sit-onyx';

import 'sit-onyx/style.css';
import '@fontsource-variable/source-code-pro';
import '@fontsource-variable/source-sans-3';

import App from './App.vue';
import router from './router';

const app = createApp(App);

app.use(createPinia());
app.use(router);
app.use(
  createOnyx({
    router: router,
  }),
);

app.mount('#app');
