export interface RegisterResult {
  status: string;
  error: string;
  registered_at: string;
  finished_at: string;
}

export class RegisterService {
  constructor(private apiURL: string) {}

  async registerEmail(tagID: string, email: string): Promise<RegisterResult> {
    console.log(tagID, email);
    return await fetch(`${this.apiURL}/email/${encodeURIComponent(tagID)}`, {
      method: 'POST',
      body: JSON.stringify({
        email: email,
      }),
      headers: {
        'Content-Type': 'application/json',
      },
    }).then((result) => result.json());
  }
}
