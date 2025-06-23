<script setup>
import Init from './views/init.vue'
import Main from './views/main.vue'
import { ref } from 'vue'

const inited = ref(false)
const deviceInfo = ref(null)
const init = (device) => {
    inited.value = true
    deviceInfo.value = device
}
</script>

<template>
    <Init v-if="!inited" @inited="init" />
    <Main v-else :deviceInfo="deviceInfo" />
</template>

<style lang="less">
body {
    margin: 0;
    padding: 0;
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    background: #000;
    color: #ffffff;
    overflow: hidden;
    user-select: none;
    -webkit-user-select: none;
    touch-action: manipulation;
}
</style>
