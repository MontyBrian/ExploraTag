<script setup>
import { ref, onMounted } from "vue";
import { useRouter } from "vue-router";
import { userManager } from "../services/auth.js";

const router = useRouter();
const error = ref("");
const status = ref("Callback läuft ...");

onMounted(async () => {
  try {
    console.log("signinCallback start");
    const user = await userManager.signinCallback();
    console.log("signinCallback ok", user);
    status.value = "Login erfolgreich";
    await router.replace("/");
  } catch (err) {
    console.error("signinCallback error", err);
    error.value = String(err?.message || err);
  }
});
</script>