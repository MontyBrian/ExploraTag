<script setup>
import { onMounted } from "vue";
import { userManager } from "@/services/auth";

onMounted(async () => {
  await userManager.signinRedirect();
});
</script>

<template>
  <div>
    Weiterleitung zum Login ...
  </div>
</template>