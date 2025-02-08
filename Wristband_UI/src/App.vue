<script setup lang="ts">
import { RouterView } from 'vue-router';
import { OnyxAppLayout, OnyxPageLayout, OnyxNavBar, OnyxToast } from 'sit-onyx';
</script>

<template>
  <OnyxAppLayout>
    <template #navBar>
      <OnyxNavBar app-name="Maker Faire Heilbronn 2025" />
    </template>

    <OnyxPageLayout>
      <RouterView />
    </OnyxPageLayout>
  </OnyxAppLayout>
  <OnyxToast />
</template>

<style scoped></style>
