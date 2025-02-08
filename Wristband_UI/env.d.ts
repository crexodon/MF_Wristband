/// <reference types="vite/client" />
declare global {
  namespace NodeJS {
    interface ProcessEnv {
      VUE_APP_API_URL: string;
    }
  }
}

export {}
