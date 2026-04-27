import { createRouter, createWebHistory } from 'vue-router'
import { userManager } from "../services/auth.js";

import Dashboard from '../views/Dashboard.vue'
import Impressum from '../views/Impressum.vue'
import LoginView from "../components/LoginView.vue";
import ProfileView from "../components/ProfileView.vue";

const routes = [
    {
        path: '/',
        name: 'dashboard',
        component: Dashboard
    },
    {
        path: '/impressum',
        name: 'impressum',
        component: Impressum
    },
    {
        path: "/login",
        name: "login",
        component: LoginView
    },
    {
        path: "/profile",
        name: "profile",
        component: ProfileView,
        meta: { requiresAuth: true }
    },
];

const router = createRouter({
    history: createWebHistory(),
    routes
});

router.beforeEach(async (to) => {
    if (!to.meta.requiresAuth) {
        return true;
    }

    const user = await userManager.getUser();

    if (user && !user.expired) {
        return true;
    }

    return "/";
});

export default router;