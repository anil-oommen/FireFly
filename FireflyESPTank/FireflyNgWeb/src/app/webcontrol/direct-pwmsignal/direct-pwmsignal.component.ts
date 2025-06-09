import { Component, OnInit } from '@angular/core';
import {coerceNumberProperty} from '@angular/cdk/coercion';
import {WebmqttService} from "../../webmqtt.service";
import {McuCommand,McuFeedback} from "../../../_models/index";

@Component({
  selector: 'app-direct-pwmsignal',
  templateUrl: './direct-pwmsignal.component.html',
  styleUrls: ['./direct-pwmsignal.component.css']
})
export class DirectPwmsignalComponent implements OnInit {

  public mcuCommand: string;
  public mcuFeedback: string;
  public dcMotorA_PWMValue:number = 0;
  public dcMotorB_PWMValue:number = 0;

  constructor(public webmqtt:WebmqttService) { }

  ngOnInit() {

    this.webmqtt.getMCUFeedbackObserve().subscribe(
      (feedback:McuFeedback)=>{
        this.mcuFeedback = JSON.stringify(feedback);
      }
    )

    this.webmqtt.getMCUCommandObserve().subscribe(
      (command:McuCommand)=>{
        this.mcuCommand = JSON.stringify(command);
      }
    )
  }

  public sendDCSingalCommand(){
    var mcuCommand = new McuCommand();
    var motorADirection = this.dcMotorA_PWMValue<0?0:1;
    var motorAPWM = this.dcMotorA_PWMValue<0?this.dcMotorA_PWMValue * -1:this.dcMotorA_PWMValue;
    var motorBDirection = this.dcMotorB_PWMValue<0?0:1;
    var motorBPWM = this.dcMotorB_PWMValue<0?this.dcMotorB_PWMValue * -1:this.dcMotorB_PWMValue;
    mcuCommand.dcMotorSignal(motorADirection,motorAPWM,motorBDirection,motorBPWM);
    this.webmqtt.sendMCURunCommand(mcuCommand);
  }
  public resetDCSignalCommand(){
    this.dcMotorA_PWMValue =0;
    this.dcMotorB_PWMValue =0;
    this.sendDCSingalCommand();
  }

  get sliderAValue(): number | 'auto' {
    return this.dcMotorA_PWMValue;
  }
  set sliderAValue(value) {
    this.dcMotorA_PWMValue = coerceNumberProperty(value);
    this.sendDCSingalCommand();
  }
  get sliderBValue(): number | 'auto' {
    return this.dcMotorB_PWMValue;
  }
  set sliderBValue(value) {
    this.dcMotorB_PWMValue = coerceNumberProperty(value);
    this.sendDCSingalCommand();
  }

}
