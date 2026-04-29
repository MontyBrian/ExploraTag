import { UserManager, WebStorageStateStore } from "oidc-client-ts"

const cognitoDomain = "https://eu-central-1twrza0itr.auth.eu-central-1.amazoncognito.com";

export const userManager = new UserManager({
    authority: cognitoDomain,
    client_id: "42rca2tfql4jgpglb3puegmecl",
    //redirect_uri: "http://localhost:5173/",
    redirect_uri: "https://d1pzkvnujvtw1y.cloudfront.net/",
    //post_logout_redirect_uri: "http://localhost:5173/",
    post_logout_redirect_uri: "https://d1pzkvnujvtw1y.cloudfront.net/",
    response_type: "code",
    scope: "openid email profile aws.cognito.signin.user.admin",
    userStore: new WebStorageStateStore({ store: window.localStorage }),
    loadUserInfo: false,
    metadata: {
        authorization_endpoint: `${cognitoDomain}/oauth2/authorize`,
        token_endpoint: `${cognitoDomain}/oauth2/token`,
        userinfo_endpoint: `${cognitoDomain}/oauth2/userInfo`,
        end_session_endpoint: `${cognitoDomain}/logout`
    }
});

export async function signOutRedirect() {
    await userManager.removeUser();

    const clientId = "42rca2tfql4jgpglb3puegmecl";
    //const logoutUri = "http://localhost:5173/";
    const logoutUri = "https://d1pzkvnujvtw1y.cloudfront.net/";
    const cognitoDomain = "https://eu-central-1twrza0itr.auth.eu-central-1.amazoncognito.com";

    window.location.href =
        `${cognitoDomain}/logout?client_id=${encodeURIComponent(clientId)}&logout_uri=${encodeURIComponent(logoutUri)}`;
}