
export class McuJoystick {
  public x: number = 0;
  public y: number = 0 ;
}

export class DCMotorSignal {
  public direction: number = 0;
  public pwm: number = 0 ;
}

export class DualDCMotor {
  public a: DCMotorSignal = new DCMotorSignal();
  public b: DCMotorSignal = new DCMotorSignal();
}

export class McuCommand {
  public joystick: McuJoystick;
  public command: string;
  public dcmotor: DualDCMotor;
  public joystickXY(x:number, y:number){
      this.joystick = new McuJoystick();
      this.joystick.x = x;
      this.joystick.y = y;
      this.command="run";
  }
  public dcMotorSignal(aDirection:number , aPWM:number , bDirection:number, bPWM:number){
    this.dcmotor = new DualDCMotor();
    this.dcmotor.a.direction= aDirection;
    this.dcmotor.a.pwm = aPWM;
    this.dcmotor.b.direction = bDirection;
    this.dcmotor.b.pwm = bPWM;
    this.command="run";
  }
  public stop(){
    this.joystick = new McuJoystick();
    this.command="stop";
  }
}

export class McuFeedback {
  public client: string;
  public event: string;
  public value: string;
  public message : string;
}