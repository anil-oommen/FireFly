import { Injectable, OnDestroy ,OnInit } from '@angular/core';
import * as createjs from 'createjs-module';
import { Subscription }  from "rxjs";
import {McuCommand,McuFeedback} from "../_models/index"
import { Observable,Subject,timer }   from 'rxjs';

import {
  IMqttMessage,
  MqttService
} from 'ngx-mqtt';
import { AngularWaitBarrier } from 'blocking-proxy/built/lib/angular_wait_barrier';

@Injectable({
  providedIn: 'root'
})
export class WebmqttService implements OnDestroy{

  public syncUpstreamToMQTT : boolean = false;
  public pauseAndHoldCommands : boolean = false;

  //lastSend_commandMcu :McuCommand = new McuCommand();
  //lastSendBeforeHold_commandMcu :McuCommand = new McuCommand();
  last_commandMcuTriggered :McuCommand = new McuCommand();

  public sendMCUStopCommand(){
    var mcuCommand = new McuCommand();
    mcuCommand.stop();
    var commandStr :string = JSON.stringify(mcuCommand);
    this.unsafePublish('firefly/dctank/control',commandStr);
    this.last_publishedMqttString=commandStr;
    this.mcuCommand.next(mcuCommand);
  }

  last_publishedMqttString : string = "";
  public sendMCURunCommand(command:McuCommand){
    this.last_commandMcuTriggered = command;
    if(this.syncUpstreamToMQTT && !this.pauseAndHoldCommands){
      var commandStr :string = JSON.stringify(command);
      if(commandStr !=this.last_publishedMqttString){
          this.unsafePublish('firefly/dctank/control',commandStr);
          this.mcuCommand.next(command);
          this.last_publishedMqttString= commandStr;
      }else{
        console.log("Ignore Was same mqttPayload");
      }
    }
  }

  

  public toggleHoldResumeMCUCommand(event:any){
    if(event.source.checked){
        //Stop and then pauseHold, else would never get out
        this.sendMCUStopCommand();
        this.pauseAndHoldCommands = true; 
    }else {
      //restore value and sent the command.
      this.pauseAndHoldCommands = false; 
      this.sendMCURunCommand(this.last_commandMcuTriggered);
    }
  }

  private subscription: Subscription;

  constructor(private _mqttService: MqttService) { 
    this.subscription = this._mqttService
        .observe('firefly/dctank/feedback')
        .subscribe((message: IMqttMessage) => {
          var feedback : McuFeedback ;
          feedback = JSON.parse(message.payload.toString());
          this.mcuFeedback.next(feedback);
          console.log(message.payload.toString());
        },
        error =>{
          console.error("ERR, Capture ");
        }
        );
  }

  ngOnInit(){

  }

  ngOnDestroy() {
    console.log('Service destroy');
    this.subscription.unsubscribe();
  }

  private mcuFeedback = new Subject<McuFeedback>();
  getMCUFeedbackObserve(): Observable<McuFeedback>{
      return this.mcuFeedback.asObservable();
  }
  private mcuCommand = new Subject<McuCommand>();
  getMCUCommandObserve(): Observable<McuCommand>{
    return this.mcuCommand.asObservable();
  }


  
  

  private unsafePublish(topic: string, message: string): void {
    this._mqttService.unsafePublish(topic, message, {qos: 1, retain: true});
  }


}
