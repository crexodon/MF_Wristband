<script setup lang="ts">
import { onMounted, ref, useTemplateRef } from 'vue';
import WristbandGreyscale from './wristband_greyscale_75px.png';
import WristbandFullcolor from './wristband_fullcolor_75px.png';

const props = defineProps<{ expectedLength: number }>();
const emit = defineEmits(['value']);
const model = defineModel<string>();

const input = useTemplateRef('input');

const focusLost = (_: FocusEvent) => {
  if (model.value?.length !== props.expectedLength) {
    model.value = '';
    setTimeout(() => input.value?.focus(), 0);
  } else {
    emit('value', model.value);
  }
};

const onKeyDown = (e: KeyboardEvent) => {
  if (e.key === 'Enter') {
    input.value?.blur();
    e.preventDefault();
  }
};

const reset = () => {
  model.value = '';
  emit('value', '');
  input.value?.focus();
};

onMounted(() => {
  input.value?.focus();
});
</script>

<template>
  <div
    class="scan-input-container"
    @click="reset"
    :style="model != '' ? { cursor: 'pointer' } : {}"
  >
    <input
      type="text"
      class="input"
      ref="input"
      autocomplete="off"
      autofocus
      @focusout="focusLost"
      @keydown="onKeyDown"
      v-model="model"
    />
    <div class="overlay">
      <img class="overlay-image" :src="WristbandGreyscale" alt="Armband scannen" />
    </div>
    <div
      class="overlay"
      :style="model?.length !== props.expectedLength ? { opacity: 0 } : { opacity: 1 }"
    >
      <img class="overlay-image" :src="WristbandFullcolor" alt="Armband erfolgreich gescannt" />
    </div>
  </div>
</template>

<style scoped>
.scan-input-container {
  position: relative;
  display: flex;
}

.input {
  flex-grow: 1;
  border: none;
  color: transparent;
  height: 75px;
}

.input:focus {
  outline: none;
}

.overlay {
  position: absolute;
  top: 0;
  left: 0;
  flex-grow: 1;

  transition: opacity 0.75s ease;
}

.overlay img {
  width: 100%;
}
</style>
